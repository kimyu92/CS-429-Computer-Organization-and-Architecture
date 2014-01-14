/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* cache description support code */

#include "global.h"
#include "cds.h"
#include "caches.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

String print_sets_and_ways(CDS *cds)
{
    if (cds->number_of_ways == 1) return("direct-mapped");
    if (cds->number_of_ways == cds->number_of_cache_entries) return("fully associative");

    static char buffer[64];
    sprintf (buffer, "%d sets of %d ways", 
             cds->number_of_cache_entries/cds->number_of_ways, cds->number_of_ways);
    return(buffer);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

String memory_reference_type_name(enum memory_access_type type)
{
    switch(type)
        {
        case MAT_FETCH: return("Fetch");
        case MAT_LOAD:  return("Load");
        case MAT_STORE: return("Store");
        }
    return("invalid");
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

String CRP_name(CDS *cds)
{
    switch(cds->replacement_policy)
        {
        case CRP_FIFO:   return("FIFO");
        case CRP_LRU:    return("LRU");
        case CRP_RANDOM: return("RANDOM");

        case CRP_LFU: 
            {
                static char buffer[64];
                sprintf(buffer, "LFU (decay=%d)", cds->LFU_Decay_Interval);
                return(buffer);
            }

        };
    return("Invalid policy");
}



void debug_print_cds(CDS *cds)
{
    fprintf(debug_file, "%s: Total number of entries: %d\n", cds->name,  cds->number_of_cache_entries);
    fprintf(debug_file, "%s: %s\n", cds->name,  print_sets_and_ways(cds));
    fprintf(debug_file, "%s: Each cache line is %d bytes\n", cds->name,  cds->cache_line_size);
    fprintf(debug_file, "%s: Cache is %s\n", cds->name,  cds->write_back ? "write-back" : "write-thru");
    fprintf(debug_file, "%s: With a %s replacement policy\n", cds->name, CRP_name(cds));
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int percent(int a, int b)
{
    if (b == 0) return(0);
    int n = (a*100/b);
    return(n);
}

void Print_Cache_Statistics_for_one_cache(CDS *cds)
{
    fprintf(stdout, "%s: %d entries of lines of %d bytes; %s, %s, %s\n",
            cds->name, cds->number_of_cache_entries, cds->cache_line_size,
            print_sets_and_ways(cds),
            cds->write_back ? "write-back" : "write-thru",
            CRP_name(cds));
            
    fprintf(stdout, "      %d addresses (%d %s, %d %s, %d %s)\n",
            cds->number_of_memory_reference, 
            cds->number_of_type[MAT_FETCH], memory_reference_type_name(MAT_FETCH), 
            cds->number_of_type[MAT_LOAD], memory_reference_type_name(MAT_LOAD), 
            cds->number_of_type[MAT_STORE], memory_reference_type_name(MAT_STORE));
    
    fprintf(stdout, "      %d hits (%d%%), %d misses, %d memory reads, %d memory writes\n",
            cds->number_cache_hits, percent(cds->number_cache_hits, cds->number_total_cache_access),
            cds->number_cache_misses,
            cds->number_memory_reads, cds->number_memory_writes);

    if (cds->write_back)
        fprintf(stdout, "      %d dirty cache lines remain\n", number_dirty_lines(cds));

    fprintf(stdout, "      %d victim cache hits", cds->number_victim_cache_hits);

    fprintf(stdout, "\n");
}


void Print_Cache_Statistics(void)
{
    CDS *cds = CDS_root;
    while (cds != NULL)
        {
            Print_Cache_Statistics_for_one_cache(cds);
            cds = cds->next;
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


void init_cache(CDS *cds)
{
    /* we need one cache line for every entry */
    cds->c = calloc(cds->number_of_cache_entries, sizeof(cache_line));
    cds->victim_c = calloc(cds->number_victim_lines, sizeof(cache_line));
}


void Init_Caches(void)
{
    CDS *cds = CDS_root;
    while (cds != NULL)
        {
            init_cache(cds);
            cds = cds->next;
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void init_cache_for_trace(CDS *cds)
{
    int i;
    for (i = 0; i < NUMBER_OF_MEMORY_ACCESS_TYPE; i++) cds->number_of_type[i] = 0;
    cds->number_of_memory_reference = 0;

    cds->number_total_cache_access = 0;
    cds->number_cache_hits = 0;
    cds->number_cache_misses = 0;
    cds->number_memory_reads = 0;
    cds->number_memory_writes = 0;
}

void Init_caches_for_trace(void)
{
    CDS *cds = CDS_root;
    while (cds != NULL)
        {
            init_cache_for_trace(cds);
            cds = cds->next;
        }
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


void delete_cache(CDS *cds)
{
    /* we need one cache line for every entry */
    free(cds->c);
    free(cds->name);
    free(cds);
}


void Delete_Caches(void)
{
    CDS *cds = CDS_root;
    while (cds != NULL)
        {
            CDS *old = cds;
            cds = cds->next;
            delete_cache(old);
        }
}


