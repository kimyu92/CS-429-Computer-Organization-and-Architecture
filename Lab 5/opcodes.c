/* 
   Assembler for PDP-8.  Opcode definition and handling.
*/

#include "asm8.h"
#include "opcode.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

opcode *Root_OP = NULL;


/* ***************************************************************** */
/*                                                                   */
opcode *search_opcode(char *name)
{
    opcode *o;
    for (o = Root_OP; o != NULL; o = o->next)
        if (strcasecmp(name, o->name) == 0) break;
    return(o);
}


/* ***************************************************************** */
/*                                                                   */
void define_opcode(char *name, enum opcode_kind kind, INST op, INST mask)
{
    opcode *o;

    o = search_opcode(name);
    if (o != NULL)
        {
            fprintf(stderr, "opcode %s redefined; old value = 0x%03X, new value = 0x%03X\n", name, o->value, op);
        }
    else
        {
            o = TYPED_MALLOC(opcode);
        }
    o->name = name;
    o->class = kind;
    o->value = op;
    o->mask = mask;
    o->next = Root_OP;
    Root_OP = o;
}


/* for each opcode, we need it's name, what type of
   opcode it is, it's numeric opcode, and what bits
   of the numeric opcode are important. */

void Initialize_Opcode_Table(void)
{
    define_opcode("AND", k_memref, 0x000, 0xE00);
    define_opcode("TAD", k_memref, 0x200, 0xE00);
    define_opcode("ISZ", k_memref, 0x400, 0xE00);
    define_opcode("DCA", k_memref, 0x600, 0xE00);
    define_opcode("JMS", k_memref, 0x800, 0xE00);
    define_opcode("JMP", k_memref, 0xA00, 0xE00);
    define_opcode("CLA", k_operate,  0xE80, 0x080);
    define_opcode("CLL", k_operate1,  0xE40, 0x140);
    define_opcode("CMA", k_operate1,  0xE20, 0x120);
    define_opcode("CML", k_operate1,  0xE10, 0x110);
    define_opcode("RAR", k_operate1,  0xE08, 0x10A);
    define_opcode("RAL", k_operate1,  0xE04, 0x106);
    define_opcode("RTR", k_operate1,  0xE0A, 0x10A);
    define_opcode("RTL", k_operate1,  0xE06, 0x106);
    define_opcode("IAC", k_operate1,  0xE01, 0x101);
    define_opcode("SMA", k_operate2,  0xF40, 0x140);
    define_opcode("SZA", k_operate2,  0xF20, 0x120);
    define_opcode("SNL", k_operate2,  0xF10, 0x110);
    define_opcode("RSS", k_operate2,  0xF08, 0x108);
    define_opcode("OSR", k_operate2,  0xF04, 0x104);
    define_opcode("HLT", k_operate2,  0xF02, 0x102);
    define_opcode("NOP", k_operate,   0xE00, 0x1FF);
    define_opcode("SPA", k_operate2,  0xF48, 0x148);
    define_opcode("SNA", k_operate2,  0xF28, 0x128);
    define_opcode("SZL", k_operate2,  0xF18, 0x118);
    define_opcode("SKP", k_operate2,  0xF08, 0x178);
    define_opcode("IOT", k_iot,      0xC00, 0x000);
    define_opcode("ORIG", k_orig,    0x000, 0x000);
    define_opcode("END", k_end,      0x000, 0x000);
    define_opcode("I", k_indirect,   0x100, 0x000);
}

