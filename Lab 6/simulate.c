
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The code to read a memory trace and simulate it on the various
   caches. */

#include "global.h"
#include "utils.h"        // utility functions
#include "cds.h"          // Cache Description Structures
#include "caches.h"       // cache manipulation code


struct memory_reference
{
    enum memory_access_type  type;
    memory_address           address;
    unsigned int             length;
};
typedef struct memory_reference memory_reference;

static int trace_line_number;



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* read on input line */

void read_reference(FILE *trace_file, memory_reference *reference)
{
    int c;
    /* we have the first character; it defined the
       memory access type.  Skip any blanks, get the
       hexadecimal address, skip the comma and get the length */

    /* skip any leading blanks */
    c = skip_blanks(trace_file);
    
    memory_address a = 0;
    while (ishex(c))
        {
            a = (a << 4) | hexvalue(c);
            c = getc(trace_file);
        }
    if (c != ',')
        {
            fprintf(stderr, "bad trace file input at line %d: %c\n", trace_line_number, c);
            exit(-1);
        }
  
    /* skip the comma */
    /* and get the length */
    int n = 0;
    c = getc(trace_file);    
    while (isdigit(c))
        {
            n = n * 10 + decvalue(c);
            c = getc(trace_file);
        }

    /* skip to end of line */
    while ((c != '\n') && (c != EOF)) c = getc(trace_file);

    /* define reference fields */
    reference->address = a;
    reference->length = n;
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


int Read_trace_file_line(FILE *trace_file, memory_reference *reference)
{
    int c;

    trace_line_number = 0;

    while ((c = getc(trace_file)) != EOF)
        {
            /* start the next line */
            trace_line_number += 1;

            /* skip any leading blanks */
            while (isspace(c) && (c != EOF)) c = getc(trace_file);

            /* what is the character ? */
            switch (c)
                {
                case 'I': /* instruction trace */
                    {
                        reference->type = MAT_FETCH;
                        read_reference(trace_file, reference);
                        return('I');
                    }

                case 'M': /* read/modify/write -- treat as a store */
                case 'S': /* store */
                    {
                        reference->type = MAT_STORE;
                        read_reference(trace_file, reference);
                        return('S');
                    }

                case 'L': /* load */
                    {
                        reference->type = MAT_LOAD;
                        read_reference(trace_file, reference);
                        return('L');
                    }
                }

            /* apparently not a reference line.  There are a bunch
               of other lines that valgrind puts out.  They start
               with  ====, or --, or such.  Skip the entire line. */
            /* skip to end of line */
            while ((c != '\n') && (c != EOF)) c = getc(trace_file);
        }
    return(EOF);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* LFU counts the number of times something is used.  To prevent
   a large number from just sitting there, we cause it to decay
   over time.  Every "tick" time units, we shift left one bit,
   so that eventually a count will go to zero if it is not continuing
   to be used. */

void Check_For_Decay(CDS *cds)
{
    if (cds->replacement_policy != CRP_LFU) return;

    if ((cds->number_of_memory_reference % cds->LFU_Decay_Interval) == 0)
        {
            int i;
            if (debug) fprintf(debug_file, "%s: LFU decay for all LFU counters\n", cds->name);
            for (i = 0; i < cds->number_of_cache_entries; i++)
                {
                    cds->c[i].replacement_data = cds->c[i].replacement_data/2;
                }
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int Find_Victim_by_Replacement_Policy(CDS *cds, int first_index, int set_size)
{
    int i;
    int victim;

    if (debug) fprintf(debug_file, "%s: look for victim in %d lines starting at %d\n", cds->name,  set_size, first_index);
    /* first look to see if any entry is empty */
    for (i = 0; i < set_size; i++)
        {
            if (!(cds->c[first_index+i].valid))
                {
                    victim = first_index+i;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
        }
    
    victim = first_index; /* default victim */
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
                    /* choose the smallest */
        case CRP_LRU:  /* replacement data is the time we were last hit */
                    /* choose the smallest */
        case CRP_LFU:  /* replacement data is the number of uses, so
                      choose the smallest */
            {
                int min = cds->c[first_index].replacement_data;
                if (debug) fprintf(debug_file, "%s: replacement data: [%d, 0x%08X]: %d", cds->name, victim, cds->c[victim].tag, min);
                for (i = 1; i < set_size; i++)
                    {
                        if (debug) fprintf(debug_file, ", [%d, 0x%08X]: %d", first_index+i, cds->c[first_index+i].tag, cds->c[first_index+i].replacement_data);
                        if (cds->c[first_index+i].replacement_data < min)
                            {
                                victim = first_index+i;
                                min = cds->c[victim].replacement_data;
                            }
                    }
                if (debug) fprintf(debug_file, "\n");
            }
            break;                     
        case CRP_RANDOM:  
            victim = first_index + (random() % set_size);
            break;
        }

    if (debug) fprintf(debug_file, "%s: found victim in entry %d\n", cds->name,  victim);
    return(victim);
}


void evict_from_victim_cache(CDS *cds, int cache_entry_index, int base_c){
    int victim = cache_entry_index;
    int i = 0;
    int min = cds->victim_c[i].replacement_data;

    for (i = 1; i < cache_entry_index; i++){    
        if (cds->victim_c[victim].replacement_data < min){
            victim = i;
            min = cds->c[victim].replacement_data;
        }
    }


}



void write_dirty_line_to_memory(CDS *cds, cache_line *victim_line)
{
    if (debug) fprintf(debug_file, "%s: Write victim 0x%08X to memory\n", 
                       cds->name,  victim_line->tag);
    cds->number_memory_writes += 1;
    victim_line->dirty = FALSE;
}


void Set_Replacement_Policy_Data(CDS *cds, int first_index, int i)
{
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
            cds->c[first_index+i].replacement_data = cds->number_of_memory_reference;
            break;
            
        case CRP_LRU:  /* replacement data is the time we were last hit */
            cds->c[first_index+i].replacement_data = cds->number_of_memory_reference;
            break;
            
        case CRP_LFU:  /* replacement data is a count; starts at zero */
            cds->c[first_index+i].replacement_data = 0;
            Check_For_Decay(cds);
            
        case CRP_RANDOM:  
            break;
        }
}

void Update_Replacement_Policy_Data(CDS *cds, int first_index, int i)
{
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
            break;
            
        case CRP_LRU:  /* replacement data is the time we were last hit */
            cds->c[first_index+i].replacement_data = cds->number_of_memory_reference;
            break;
            
        case CRP_LFU:  /* replacement data is the count of the number of uses */
            cds->c[first_index+i].replacement_data += 1;
            Check_For_Decay(cds);
            
        case CRP_RANDOM:  
            break;
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void evict_from_cache(CDS *cds, cache_line *victim_line, memory_address cache_address)
{
    /* if victim is dirty, must write back to memory first */
    if (victim_line->dirty){
        write_dirty_line_to_memory(cds, victim_line);
    }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

memory_address Base_Cache_Address(CDS *cds, memory_address a)
{
    /* find number of low-order bits to mask off to find beginning cache
       line address */
    int number_of_low_order_bits = which_power(cds->cache_line_size);
    memory_address low_order_mask = mask_of(number_of_low_order_bits);
    memory_address cache_address = a & (~low_order_mask);
    return(cache_address);
}


void Simulate_Reference_to_Cache_Line(CDS *cds, memory_reference *reference)
{
    if (debug) fprintf(debug_file, "%s: %s Reference 0x%08X of length %d\n",
                       cds->name, memory_reference_type_name(reference->type),
                       reference->address, reference->length);

    cds->number_total_cache_access += 1;

    /* find cache line for this reference */
    /* find number of low-order bits to mask off to find beginning cache
       line address */
    
    memory_address cache_address = Base_Cache_Address(cds, reference->address);

    /* shift off low-order offset bits and find bits for 
       indexing into cache table */
    /* the number of sets is the number of cache entries
       divided by the number of ways. */
    int number_of_low_order_bits = which_power(cds->cache_line_size);
    int number_of_sets = cds->number_of_cache_entries/cds->number_of_ways;
    int sets_bits = which_power(number_of_sets);
    memory_address sets_bits_mask = mask_of(sets_bits);
    int cache_set_index = (cache_address >> number_of_low_order_bits) & sets_bits_mask;
    int cache_entry_index = cache_set_index * cds->number_of_ways;
    
    /* index into cache table and search the number of ways to
       try to find cache line. */
    int i;
    for (i = 0; i < cds->number_of_ways; i++)
        {
            if (cds->c[cache_entry_index+i].valid && (cache_address == cds->c[cache_entry_index+i].tag))
                {
                    /* found it -- record cache hit and exit */
                    if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name, 
                                       reference->address, cache_entry_index+i);
                    cds->number_cache_hits += 1;

                    /* update reference specific info */
                    if (reference->type == MAT_STORE) 
                        {
                            /* If it's not write-back, then it is write-thru.
                               For write-thru, if it's a write, we write to memory. */
                            if (!cds->write_back)
                                cds->number_memory_writes += 1;
                            else
                                /* For write-back, if it's a write, it's dirty. */
                                cds->c[cache_entry_index+i].dirty = TRUE;
                        }
                    
                    Update_Replacement_Policy_Data(cds, cache_entry_index, i);
                    return;
                }
        }

    /* read cache line from memory into cache table */
    cds->number_cache_misses += 1;
    cds->number_memory_reads += 1;

    
    /* Did not find it. */
    i = 0;
    while (i < (cds->number_victim_lines) ){
        if (cds->victim_c[i].valid && (cache_address == cds->victim_c[i].tag)){
            //found it
            //Grab the index
            int base_c = Find_Victim_by_Replacement_Policy(cds, cache_entry_index, cds->number_of_ways);
            //Swapping
            cache_line temp = cds->victim_c[i];
            cds->victim_c[i] = cds->c[base_c];
            cds->c[base_c] = temp;

            cds->number_victim_cache_hits += 1;

            if (reference->type == MAT_STORE) {
            /* If it's not write-back, then it is write-thru.
            For write-thru, if it's a write, we write to memory. */
            
                if (!cds->write_back)
                    cds->number_memory_writes += 1;
                else
                    /* For write-back, if it's a write, it's dirty. */
                    cds->victim_c[i].dirty = TRUE;
            }
            Update_Replacement_Policy_Data(cds, cache_entry_index, i);
            return;
        }
        i++;
    }


    /* Choose a victim from the set */
    int victim = Find_Victim_by_Replacement_Policy(cds, cache_entry_index, cds->number_of_ways);
    if (debug) fprintf(debug_file, "%s: Pick victim %d to replace\n", cds->name,  victim);
    
    /* evict victim */
    if (cds->c[victim].valid){
        evict_from_victim_cache(cds, cache_entry_index, victim);
        evict_from_cache(cds, &(cds->c[victim]), cache_address);
    }

    
    /* fill in evicted cache line for this new line */
    cds->c[victim].valid = TRUE;
    cds->c[victim].tag = cache_address;
    if (debug) fprintf(debug_file, "%s: Read cache line 0x%08X into entry %d\n", cds->name,  cds->c[victim].tag, victim);

    Set_Replacement_Policy_Data(cds, cache_entry_index, victim-cache_entry_index);



    cds->c[victim].dirty = FALSE;
    if (reference->type == MAT_STORE)
        {
            if (!cds->write_back)
                {
                    /* If it's not write-back, then it is write-thru.
                       For write-thru, if it's a write, we write to memory. */
                    cds->number_memory_writes += 1;
                    if (debug) fprintf(debug_file, "%s: Write cache line 0x%08X thru to memory\n", cds->name,  cds->c[victim].tag);
                }
            else
                {
                    cds->c[victim].dirty = TRUE;
                }
        }
}

void Simulate_Reference_to_Memory(CDS *cds, memory_reference *reference)
{
    cds->number_of_memory_reference += 1;
    cds->number_of_type[reference->type] += 1;

    /* check if the entire reference fits into just one cache line */
    if (Base_Cache_Address(cds, reference->address) == Base_Cache_Address(cds, reference->address + reference->length -1))
        {
            Simulate_Reference_to_Cache_Line(cds, reference);
        }
    else
        {
            /* reference spans two cache lines.  Convert it to two
               references: the first cache line, and the second cache line */
            memory_reference reference1;
            memory_reference reference2;
            /* easiest to compute the second part first */
            reference2.type = reference->type;
            reference2.address = Base_Cache_Address(cds, reference->address + reference->length -1);
            reference2.length = reference->address + reference->length - reference2.address;

            reference1.type = reference->type;
            reference1.address = reference->address;
            reference1.length = reference->length - reference2.length;

            /* but we do the references first, then second */
            Simulate_Reference_to_Cache_Line(cds, &reference1);
            Simulate_Reference_to_Cache_Line(cds, &reference2);
        }
}




/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* read each input line, and then simulate that reference on each
   cache. */

void Simulate_Caches(String trace_file_name)
{
    FILE *trace_file;
    memory_reference reference;

    /* open input file */
    trace_file = fopen(trace_file_name, "r");
    if (trace_file == NULL)
        {
            fprintf (stderr,"Cannot open trace file %s\n", trace_file_name);
            exit(1);
        }

    Init_caches_for_trace();

    while (Read_trace_file_line(trace_file, &reference) != EOF)
        {
            CDS *cds = CDS_root;
            while (cds != NULL)
                {
                    Simulate_Reference_to_Memory(cds, &reference);
                    cds = cds->next;
                }
        }
    fclose(trace_file);
}


int number_dirty_lines(CDS *cds)
{
    int n = 0;
    int i;
    for (i = 0; i < cds->number_of_cache_entries; i++)
        {
            if (cds->c[i].dirty)
                {
                    n += 1;
                    if (debug) fprintf(debug_file, "%s: Cache Line 0x%08X is dirty\n", cds->name,  cds->c[i].tag);
                }
        }
    return(n);
}
