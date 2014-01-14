/* 
   Assembler for PDP-8.  Symbol table type definitions
*/

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

struct forward_reference_node
{
    struct forward_reference_node *next;
    Address addr;
    Boolean full;
    int line_number;
};


struct symbol_table_entry
{
    struct symbol_table_entry *next;
    char    *name;
    Address  value;
    struct forward_reference_node *fr;
};

typedef struct symbol_table_entry symbol;

/* function prototypes */

symbol *search_symbol(char *name);

void define_symbol(char *name, Address value);

void forward_reference(char *name, int line_number, Address reference_address, Boolean full);

void Check_for_undefined_symbols(void);

#endif
