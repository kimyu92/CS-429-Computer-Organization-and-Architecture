/* 
   Assembler for PDP-8.  Opcode type definitions.
*/

#ifndef _OPCODE_H_
#define _OPCODE_H_

#include "asm8.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

enum opcode_kind {k_memref, k_operate, k_operate1, k_operate2, k_iot, k_orig, k_end, k_indirect};

struct opcode_table_entry
{
    struct opcode_table_entry *next;
    const char       *name;
    enum opcode_kind  class;
    INST  value;
    INST  mask;
};
typedef struct opcode_table_entry opcode;

/* function prototypes */

void Initialize_Opcode_Table(void);
opcode *search_opcode(char *name);


#endif
