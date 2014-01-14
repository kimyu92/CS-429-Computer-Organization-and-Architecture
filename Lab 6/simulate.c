
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

void Check_For_Decay(counter_t time, struct cache *c)
{
    if (c->replacement_policy != CRP_LFU) return;

    if ((time % c->LFU_Decay_Interval) == 0)
        {
            int i;
            if (debug) fprintf(debug_file, "%s: LFU decay for all LFU counters\n", c->name);
            for (i = 0; i < c->number_of_cache_entries; i++)
                {
                    c->c_line[i].replacement_data = c->c_line[i].replacement_data/2;
                }
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

memory_address Base_Cache_Address(struct cache *c, memory_address a)
{
    /* find number of low-order bits to mask off to find beginning cache
       line address */
    int number_of_low_order_bits = which_power(c->cache_line_size);
    memory_address low_order_mask = mask_of(number_of_low_order_bits);
    memory_address cache_address = a & (~low_order_mask);
    return(cache_address);
}

int Compute_Set_Index(struct cache *c, memory_address cache_address)
{
    /* shift off low-order offset bits and find bits for 
       indexing into cache table */
    /* the number of sets is the number of cache entries
       divided by the number of ways. */
    int number_of_low_order_bits = which_power(c->cache_line_size);
    int number_of_sets = c->number_of_cache_entries/c->number_of_ways;
    int sets_bits = which_power(number_of_sets);
    memory_address sets_bits_mask = mask_of(sets_bits);
    int cache_set_index = (cache_address >> number_of_low_order_bits) & sets_bits_mask;
    int cache_entry_index = cache_set_index * c->number_of_ways;
    return(cache_entry_index);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* search in the cache for the particular cache address we want */
int Search_Cache_For(struct cache *c, memory_address cache_address)
{
    int cache_entry_index = Compute_Set_Index(c, cache_address);
    
    if (debug) fprintf(debug_file, "%s: search cache lines %d to %d for 0x%08X\n",
                       c->name, cache_entry_index, 
                       cache_entry_index+c->number_of_ways-1, cache_address);

    /* index into cache table and search the number of ways to
       try to find cache line. */
    int i;
    for (i = 0; i < c->number_of_ways; i++)
        {
            if (c->c_line[cache_entry_index+i].valid 
                && (cache_address == c->c_line[cache_entry_index+i].tag))
                {
                    c->number_cache_hits += 1;
                    return(cache_entry_index+i);
                }
        }
    c->number_cache_misses += 1;    
    return(-1);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int Find_Victim_by_Replacement_Policy(struct cache *c, memory_address cache_address)
{
    int i;
    int victim;
    
    int first_index = Compute_Set_Index(c, cache_address);
    int set_size = c->number_of_ways;
    if (debug) fprintf(debug_file, "%s: look for victim in %d lines starting at %d\n", c->name,  set_size, first_index);
    
    /* first look to see if any entry is empty */
    for (i = 0; i < set_size; i++)
        {
            if (!(c->c_line[first_index+i].valid))
                {
                    victim = first_index+i;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", c->name,  victim);
                    return(victim);
                }
        }
    
    /* No empty cache line entry */
    victim = first_index; /* default victim */
    switch (c->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
                    /* choose the smallest */
            
        case CRP_LRU:  /* replacement data is the time we were last hit */
                    /* choose the smallest */

        case CRP_LFU:  /* replacement data is the number of uses, so
                      choose the smallest */
            {
                int min = c->c_line[first_index].replacement_data;
                if (debug) fprintf(debug_file, "%s: replacement data: [%d, 0x%08X]: %d", c->name, victim, c->c_line[victim].tag, min);
                for (i = 1; i < set_size; i++)
                    {
                        if (debug) fprintf(debug_file, ", [%d, 0x%08X]: %d", first_index+i, c->c_line[first_index+i].tag, c->c_line[first_index+i].replacement_data);
                        if (c->c_line[first_index+i].replacement_data < min)
                            {
                                victim = first_index+i;
                                min = c->c_line[victim].replacement_data;
                            }
                    }
                if (debug) fprintf(debug_file, "\n");
            }
            break;
                              
        case CRP_RANDOM:  
            victim = first_index + (random() % set_size);
            break;
        }

    if (debug) fprintf(debug_file, "%s: found victim in entry %d\n", c->name,  victim);
    return(victim);
}


void evict_dirty_line_from_cache(struct cache *c, cache_line *victim_line)
{
    if (debug) fprintf(debug_file, "%s: Write dirty victim 0x%08X\n", 
                       c->name,  victim_line->tag);
    c->number_miss_writes += 1;
}


void Set_Replacement_Policy_Data(counter_t time, struct cache *c, cache_line *cache_entry)
{
    switch (c->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
            cache_entry->replacement_data = time;
            break;
            
        case CRP_LRU:  /* replacement data is the time we were last hit */
            cache_entry->replacement_data = time;
            break;
            
        case CRP_LFU:  /* replacement data is a count; starts at zero */
            cache_entry->replacement_data = 0;
            Check_For_Decay(time, c);
            
        case CRP_RANDOM:  
            break;
        }
}

void Update_Replacement_Policy_Data(counter_t time, struct cache *c, cache_line *cache_entry)
{
    switch (c->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
            break;
            
        case CRP_LRU:  /* replacement data is the time we were last hit */
            cache_entry->replacement_data = time;
            break;
            
        case CRP_LFU:  /* replacement data is the count of the number of uses */
            cache_entry->replacement_data += 1;
            Check_For_Decay(time, c);
            
        case CRP_RANDOM:  
            break;
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#define swap1(a,b) { int t = a; a = b; b = t; }

void swap_cache_lines(cache_line *a, cache_line *b)
{
    swap1(a->tag, b->tag);
    swap1(a->dirty, b->dirty);
}


Boolean evict_from_cache(CDS *cds, cache_line *victim_line, memory_address cache_address)
{
    /* if victim is dirty, note that this dirty line is being evicted */
    if (victim_line->dirty){
        evict_dirty_line_from_cache(cds->c, victim_line);
    }
            
    return(FALSE);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
Boolean evict_from_victim_cache(CDS *cds, cache_line *victim_line, memory_address cache_address, int cache_entry_index, memory_reference *reference){
    //here is where we search for the victim cache
    Boolean Found = FALSE;

    int victim_cache_entry_index = Search_Cache_For(cds->v, cache_address);

    cache_line *cache_entry = &(cds->c->c_line[cache_entry_index]);
    cache_line *victim_cache_entry = &(cds->v->c_line[victim_cache_entry_index]);

    
    //Find it in victim cache
    if (victim_cache_entry_index >= 0){
        Found = TRUE;
        swap_cache_lines(victim_cache_entry, cache_entry);
    }
    else{

        victim_cache_entry_index = Find_Victim_by_Replacement_Policy(cds->v, cache_address);
        victim_cache_entry = &(cds->v->c_line[victim_cache_entry_index]);

        //Before writing inside into victim line
        //If it is dirty, write it down
        if(victim_cache_entry->dirty){
            evict_dirty_line_from_cache(cds->v, victim_cache_entry);
        }
        


        //So, we can safely write the victim (base cache) into the victim cache
        if(!Found){
            victim_cache_entry->tag = cache_address;
            victim_cache_entry->valid = TRUE;
            victim_cache_entry->dirty = cache_entry->dirty;

            cds->v->c_line[victim_cache_entry_index] = cds->c->c_line[cache_entry_index];

            //victim miss read
            //Have to grab from the memory
            cds->v->number_miss_reads += 1;
        }

    }

    Set_Replacement_Policy_Data(cds->number_of_memory_reference, cds->v, victim_cache_entry);


    return Found;

}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void Simulate_Reference_to_Cache_Line(CDS *cds, memory_reference *reference)
{
    Boolean Found = FALSE;
    cache_line *cache_entry = NULL;

    if (debug) fprintf(debug_file, "%s: %s Reference 0x%08X of length %d\n",
                       cds->name, memory_reference_type_name(reference->type),
                       reference->address, reference->length);
    
    cds->c->number_total_cache_access += 1;
    
    /* find cache line for this reference */
    /* find number of low-order bits to mask off to find beginning cache
       line address */
    
    memory_address cache_address = Base_Cache_Address(cds->c, reference->address);
    
    int cache_entry_index = Search_Cache_For(cds->c, cache_address);
    if (cache_entry_index >= 0){
        /* found it -- record cache hit */
    	Found = TRUE;
    	
        if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name, 
    		reference->address, cache_entry_index);
    		cache_entry = &(cds->c->c_line[cache_entry_index]);
    }
    else{
        /* Did not find it. */
    	Found = FALSE;

        /* Choose a victim from the set */
    	cache_entry_index = Find_Victim_by_Replacement_Policy(cds->c, cache_address);
    	cache_entry = &(cds->c->c_line[cache_entry_index]);

        
    	if (debug) fprintf(debug_file, "%s: Pick victim %d to replace\n", cds->name,  cache_entry_index);

            /* evict victim */
    	if (cache_entry->valid){
            if(cds->victim_cache_is_created == TRUE){
                //Access victim cache
                evict_from_cache(cds, cache_entry, cache_address);
                cds->v->number_total_cache_access += 1;
                Found = evict_from_victim_cache(cds, cache_entry, cache_address, cache_entry_index, reference);
            }
            else{
                Found = evict_from_cache(cds, cache_entry, cache_address);
            }
    	}

    	
    	if (!Found){
            /* fill in evicted cache line for this new line */
    		cache_entry->valid = TRUE;
    		cache_entry->tag = cache_address;
    		cache_entry->dirty = FALSE;

            /* read cache line from memory into cache table */
    		if (debug) fprintf(debug_file, "%s: Read cache line 0x%08X into entry %d\n", cds->name,  cache_entry->tag, cache_entry_index);
    		cds->c->number_miss_reads += 1;
    	}
    }

    /* update reference specific info */
    if (reference->type == MAT_STORE) {
        /* If it's not write-back, then it is write-thru.
        For write-thru, if it's a write, we write to memory. */
        
        if (!cds->c->write_back){
            cds->c->number_miss_writes += 1;
            if (debug) fprintf(debug_file, "%s: Write cache line 0x%08X thru to memory\n", cds->name,  cache_entry->tag);
        }
        else
            /* For write-back, if it's a write, it's dirty. */
            cache_entry->dirty = TRUE;
    }

    
    if (!Found)
        Set_Replacement_Policy_Data(cds->number_of_memory_reference, cds->c, cache_entry);
    else
        Update_Replacement_Policy_Data(cds->number_of_memory_reference, cds->c, cache_entry);
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
void Simulate_Reference_to_Memory(CDS *cds, memory_reference *reference)
{
    cds->number_of_memory_reference += 1;
    cds->number_of_type[reference->type] += 1;

    /* check if the entire reference fits into just one cache line */
    if (Base_Cache_Address(cds->c, reference->address) == Base_Cache_Address(cds->c, reference->address + reference->length -1)){
        Simulate_Reference_to_Cache_Line(cds, reference);
    }
    else{
        /* reference spans two cache lines.  Convert it to two
        references: the first cache line, and the second cache line */
        memory_reference reference1;
        memory_reference reference2;
        
        /* easiest to compute the second part first */
        reference2.type = reference->type;
        reference2.address = Base_Cache_Address(cds->c, reference->address + reference->length -1);
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
void Simulate_Caches(String trace_file_name){
    FILE *trace_file;
    memory_reference reference;

    /* open input file */
    trace_file = fopen(trace_file_name, "r");
    if (trace_file == NULL){
        fprintf (stderr,"Cannot open trace file %s\n", trace_file_name);
        exit(1);
    }

    Init_caches_for_trace();

    while (Read_trace_file_line(trace_file, &reference) != EOF){
        CDS *cds = CDS_root;
        
        while (cds != NULL){
            Simulate_Reference_to_Memory(cds, &reference);
            cds = cds->next;
        }
    }

    fclose(trace_file);
}


int number_dirty_lines(struct cache *c){
    int n = 0;
    int i;
    
    for (i = 0; i < c->number_of_cache_entries; i++){
        if (c->c_line[i].dirty){
            n += 1;
            if (debug) fprintf(debug_file, "%s: Cache Line 0x%08X is dirty\n", c->name, c->c_line[i].tag);
        }
    }
    
    return(n);
}
