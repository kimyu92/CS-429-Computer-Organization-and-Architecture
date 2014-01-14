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

String print_sets_and_ways(struct cache *c)
{
    if (c->number_of_ways == 1) return("direct-mapped");
    if (c->number_of_ways == c->number_of_cache_entries) return("fully associative");

    static char buffer[64];
    sprintf (buffer, "%d sets of %d ways", 
             c->number_of_cache_entries/c->number_of_ways, c->number_of_ways);
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

String CRP_name(struct cache *c)
{
    switch(c->replacement_policy)
        {
        case CRP_FIFO:   return("FIFO");
        case CRP_LRU:    return("LRU");
        case CRP_RANDOM: return("RANDOM");

        case CRP_LFU: 
            {
                static char buffer[64];
                sprintf(buffer, "LFU (decay=%d)", c->LFU_Decay_Interval);
                return(buffer);
            }

        };
    return("Invalid policy");
}



void debug_print_cache(struct cache *c)
{
    fprintf(debug_file, "%s: Total number of entries: %d\n", c->name,  c->number_of_cache_entries);
    fprintf(debug_file, "%s: %s\n", c->name,  print_sets_and_ways(c));
    fprintf(debug_file, "%s: Each cache line is %d bytes\n", c->name,  c->cache_line_size);
    fprintf(debug_file, "%s: Cache is %s\n", c->name,  c->write_back ? "write-back" : "write-thru");
    fprintf(debug_file, "%s: With a %s replacement policy\n", c->name, CRP_name(c));
}


void debug_print_cds(CDS *cds)
{
    debug_print_cache(cds->c);
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


void Print_Cache_Statistics_for_one_cache(struct cache *c)
{
    fprintf(stdout, "%s: %d entries of lines of %d bytes; %s, %s, %s\n",
            c->name, c->number_of_cache_entries, c->cache_line_size,
            print_sets_and_ways(c),
            c->write_back ? "write-back" : "write-thru",
            CRP_name(c));
    
    fprintf(stdout, "%s: %d accesses, %d hits (%d%%), %d misses, %d miss reads, %d miss writes\n",
            c->name, c->number_total_cache_access,
            c->number_cache_hits, percent(c->number_cache_hits, c->number_total_cache_access),
            c->number_cache_misses, c->number_miss_reads, c->number_miss_writes);
    
    if (c->write_back)
        fprintf(stdout, "%s: %d dirty cache lines remain\n", c->name, number_dirty_lines(c));
}


void Print_Cache_Statistics_for_one_cds(CDS *cds)
{
    fprintf(stdout, "      %d addresses (%d %s, %d %s, %d %s)\n",
            cds->number_of_memory_reference, 
            cds->number_of_type[MAT_FETCH], memory_reference_type_name(MAT_FETCH), 
            cds->number_of_type[MAT_LOAD], memory_reference_type_name(MAT_LOAD), 
            cds->number_of_type[MAT_STORE], memory_reference_type_name(MAT_STORE));
    
    Print_Cache_Statistics_for_one_cache(cds->c);
    Print_Cache_Statistics_for_one_cache(cds->v);
    
    fprintf(stdout, "\n");
}


void Print_Cache_Statistics(void)
{
    CDS *cds = CDS_root;
    while (cds != NULL)
        {
            Print_Cache_Statistics_for_one_cds(cds);
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
    cds->c->c_line = CAST(cache_line *, calloc(cds->c->number_of_cache_entries, sizeof(cache_line)));
    //initialize the cache
    cds->v->c_line = CAST(cache_line *, calloc(cds->c->number_of_cache_entries, sizeof(cache_line)));
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
    cds->c->number_miss_reads = 0;
    cds->c->number_miss_writes = 0;

    cds->c->number_total_cache_access = 0;
    cds->c->number_cache_hits = 0;
    cds->c->number_cache_misses = 0;
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


void delete_one_cache(struct cache *c)
{
    free(c->c_line);
    free(c->name);
}

void delete_cache(CDS *cds)
{
    /* we need one cache line for every entry */
    delete_one_cache(cds->c);

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


