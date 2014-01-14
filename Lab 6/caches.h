
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include "global.h"
#include "cds.h"

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

extern void Read_Cache_Descriptions(String CDS_file_name);

/* ***************************************************************** */
            
extern void debug_print_cds(CDS *cds);

extern void Print_Cache_Statistics(void);
extern int number_dirty_lines(CDS *cds);
extern void Init_Caches(void);
extern void Init_caches_for_trace(void);
extern void Delete_Caches(void);

/* ***************************************************************** */

extern void Simulate_Caches(String trace_file_name);

