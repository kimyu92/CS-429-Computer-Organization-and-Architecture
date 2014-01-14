/* 
   Assembler for PDP-8.  Symbol table and forward references.
*/

#include "asm8.h"
#include "symbol.h"
#include "objmem.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

symbol *Root_ST = NULL;

symbol *search_symbol(char *name)
{
    symbol *s;
    for (s = Root_ST; s != NULL; s = s->next)
        if (strcasecmp(name, s->name) == 0) break;

    return(s);
}

symbol *insert_symbol(char *name)
{
    symbol *s;
    
    s = TYPED_MALLOC(symbol);
    s->name = remember_string(name);
    s->value = 0;
    s->fr = NULL;
    s->next = Root_ST;
    Root_ST = s;
    
    return(s);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void  handle_forward_references(symbol *s)
{
    /* plug the new address into those instructions
       that referenced it */
    while (s->fr != NULL)
        {
            /* remove the forward note from the list */
            struct forward_reference_node *fr = s->fr;
            s->fr = fr->next;
            if (fr->full)
                Define_Object_Code(fr->addr, s->value, TRUE);
            else
                {
                    INST inst = Fetch_Object_Code(fr->addr);
                    inst = Adjust_for_ZC(fr->addr, inst, s->value);
                    Define_Object_Code(fr->addr, inst, TRUE);
                }
            free(fr);
        }
}

void forward_reference(char *name, int line_number, Address reference_address, Boolean full)
{
    if (debug) fprintf(stderr, "%s forward reference to %s at line %d, address 0x%03X\n", 
                       (full ? "full" : "page"),
                       name, line_number, reference_address);

    /* get a symbol table entry; define one if necessary */
    symbol *s = search_symbol(name);
    if (s == NULL)
        s = insert_symbol(name);
    
    struct forward_reference_node *fr = TYPED_MALLOC(struct forward_reference_node);
    fr->addr = reference_address;
    fr->full = full;
    fr->line_number = line_number;

    /* link this into the list */
    fr->next = s->fr;
    s->fr = fr;
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


void define_symbol(char *name, Address value)
{
    symbol *s = search_symbol(name);

    if (s == NULL)
        s = insert_symbol(name);
    else if (s->fr == NULL)
        {
            number_of_errors += 1;
            fprintf(stderr, "symbol %s redefined; old value = 0x%03X, new value = 0x%03X\n", name, s->value, value);
        }
    else
        {
            if (debug) fprintf(stderr, "symbol %s defined; value = 0x%03X\n", s->name, s->value);
        }
    
    /* define the new value */
    s->value = value;

    /* if there were forward references, walk the list and define them */
    if (s->fr != NULL)
        {
            handle_forward_references(s);
        }
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


void Check_for_undefined_symbols(void)
{
    /* walk the entire symbol table */
    /* look to see if anyone has an non-empty list of forward references */
    symbol *s;
    for (s = Root_ST; s != NULL; s = s->next)
        if (s->fr != NULL)
            {
                number_of_errors += 1;
                fprintf(stderr, "Undefined symbol %s used at line %d\n",
                        s->name, s->fr->line_number);
            }
}


