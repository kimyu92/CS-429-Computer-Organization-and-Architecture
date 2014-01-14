//Name   : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA     : Ji Hong
/*
  PDP-8 Assembler:

  input is an assembly language program.  Each line is:

      [label]:   opcodes    [operands]     // comment

   opcodes include:

   mem-ref:  TAD, AND, ISZ, DCA, JMS, JMP
   operate:  CLA, CLL, CMA, CML, RAR, RAL, IAC, SMA, SZA, SNL, OSR, HLT,
             NOP, SPA, SNA, SZL, SKP  RTR, RTL
   i/o:      IOT
   pseudo:   ORIG, END, INT

   symbols are labels and constants -- decimal, octal, hex, char
*/

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include "asm8.h"
#include "token.h"
#include "symbol.h"
#include "opcode.h"
#include "objmem.h"

Boolean debug = FALSE;
int number_of_errors = 0;

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* MALLOC space for a string and copy it */

STRING remember_string(const STRING name)
{
    size_t n;
    STRING p;

    if (name == NULL) return(NULL);

    /* get memory to remember file name */
    n = strlen(name) + 1;
    p = CAST(STRING, malloc(n));
    strcpy(p, name);
    return(p);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

INST Adjust_for_ZC(Address pc, INST instruction, Address addr)
{
    Address addr_page = (addr & 0xF80);
    Address curr_page = (pc & 0xF80);
    /* set the Z/C bit for this address */
    if (addr_page == 0)
        {
            /* set Z/C bit to zero */
            instruction = instruction & ~(0x080);
        }
    else if (addr_page == curr_page)
        {
            /* set Z/C bit to one */
            instruction = instruction | 0x080;
        }
    else
        {
            number_of_errors += 1;
            fprintf(stderr, "Address is not on zero or current page: Address page = 0x%3X, Current page = 0x%3X\n", 
                    addr_page, curr_page);
        }
    
    /* add in address page offset to the instruction */
    instruction = instruction | (addr & 0x07F);

    return(instruction);
}


/* ***************************************************************** */

/* where the current instruction should go in memory */
Address location_counter = 0;

/* The instruction being assembled */
INST instruction;

/* As we assemble the instruction, the bits may be either fixed
   (as either 0 or 1), or not yet known.  For example if we have
   SMA, we know it is an operate group 2, and the SMA bit is set,
   but we don't know which (if any) of the other group 2 opcodes
   will be set.  So we know the opcode (0x111...)  and the group
   2 opcode bit (0x...1....), and the SMA bit (0x.....1...), but
   the other bits are not known (and hence are not yet fixed).
*/
INST fixed_bits;


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void do_opcode(Token *t)
{
    switch (t->op->class)
        {
        case k_indirect:
            good_stuff = FALSE;
            number_of_errors += 1;
            fprintf(stderr, "Missing memory reference opcode with apparent indirect address\n");
            break;

        case k_memref:
            {
                Address addr;
                
                good_stuff = TRUE;
                /* first the opcode */
                instruction = t->op->value;
                
                /* check for indirect */
                get_token(t);
                if ((t->type == Topcode) && (t->op->class == k_indirect))
                    {
                        instruction = instruction | t->op->value;
                        get_token(t);
                    }
                
                if (t->type == Tconstant) 
                    {
                        addr = t->value;
                    }
                else if (t->type == Tsymbol)
                    {
                        if ((t->sy == NULL) || (t->sy->fr != NULL))
                            {
                                forward_reference(t->token_string, line_number, location_counter, FALSE);
                            }
                        addr = t->value;
                    }
                else
                    {
                        number_of_errors += 1;
                        fprintf(stderr, "Memory reference instruction operand must be constant or symbol\n");
                        addr = 0;
                    }

                instruction = Adjust_for_ZC(location_counter, instruction, addr);

                /* new token for further processing */
                get_token(t);
                break;
            }

        case k_operate: 
        case k_operate1:
        case k_operate2:
            good_stuff = TRUE;
            /* if we have operate1 or operate2, then we know the
               top 4 bits; otherwise we only know the top 3 bits (so far) */
            if (t->op->class == k_operate)
                fixed_bits = 0xE00;
            else
                fixed_bits = 0xF00;
            instruction = t->op->value & fixed_bits;

            /* get all the opcodes for the micro-opcodes and OR them together */
            /* but don't allow opcodes that define bits in conflict with each other */
            do {
                /* check if more than one wants to define some bits */
                INST conflicts = fixed_bits & t->op->mask;
                if (conflicts != 0)
                    {
                        /* see if they both want the bits the same, or different */
                        if ((instruction & conflicts) != (t->op->value & conflicts))
                            {
                                number_of_errors += 1;
                                fprintf(stderr, "incompatible opcodes at line %d\n", line_number);
                            }
                    }
                instruction = instruction | t->op->value;
                fixed_bits = fixed_bits | t->op->mask;
                
                get_token(t);
            } while (t->type == Topcode);
            break;
            
        case k_iot:
            {
                int device = 0;
                int function = 0;

                /* IO instruction;  IOT device function */
                good_stuff = TRUE;
                instruction = t->op->value;
                
                get_token(t);
                if (t->type != Tconstant)
                    {
                        number_of_errors += 1;
                        fprintf(stderr, "IOT device operand must be constant\n");
                    }
                else
                    device = t->value;
                instruction = instruction | ((device & 0x3F) << 3);

                get_token(t);

                if(t->type == Tcolon)
                    get_token(t);
                
                if (t->type != Tconstant)
                    {
                        number_of_errors += 1;
                        fprintf(stderr, "IOT function operand must be constant\n");
                    }
                else
                    function = t->value;
                instruction = instruction | (function & 0x3);

                /* get the next token, for the return */
                get_token(t);
                break;
            }
                
        case k_orig:
            /* ORIG value */
            good_stuff = FALSE;

            get_token(t);
            if (t->type != Tconstant)
                {
                    number_of_errors += 1;
                    fprintf(stderr, "ORIG operand must be constant\n");
                }
            else
                location_counter = t->value;

            /* get the next token, for the return */
            get_token(t);
            break;

        case k_end:
            /* END value  or END symbol */
            good_stuff = FALSE;

            get_token(t);
            if (t->type == Tconstant) 
                {
                    entry_point = t->value;
                }
            else if (t->type == Tsymbol)
                {
                    if ((t->sy == NULL) || (t->sy->fr != NULL))
                        {
                            forward_reference(t->token_string, line_number, location_counter, TRUE);
                        }
                    entry_point = t->value;
                }
            else
                {
                    number_of_errors += 1;
                    fprintf(stderr, "END operand must be constant or symbol\n");
                }

            /* get the next token, for the return */
            get_token(t);
            break;

        }

}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Assemble_File(STRING name)
{
    Token t1;
    t1.token_string = NULL;

    number_of_errors = 0;
    line_number = 0;

    while (get_next_line() != EOF)
        {
            /* get the token to process */
            get_token(&t1);

            /* look at the next token, is this a label ? */
            enum Token_type t = peek_token_type();
            while (t == Tcolon)
                {
                    define_symbol(t1.token_string, location_counter);
                    /* skip colon */
                    get_token(&t1);
                    /* and get the next symbol (if any) */
                    get_token(&t1);
                    t = peek_token_type();
                }

            /* starting an instruction */
            good_stuff = FALSE;
            instruction = 0;
            fixed_bits = 0;

            while ((t1.type == Topcode) || (t1.type == Tsymbol) || (t1.type == Tconstant))
                {
                    /* not a label -- do what you can for it */
                    switch (t1.type)
                        {
                        case Topcode:
                            /* if we already have good stuff here, why do we have another opcode? */
                            if (good_stuff)
                                {
                                    number_of_errors += 1;
                                    fprintf(stderr, "yet another opcode at line %d\n", line_number);
                                }

                            do_opcode(&t1);
                            /* do_opcode will advance the token */
                            break;
                            
                        case Tsymbol:
                            if ((t1.sy == NULL) || (t1.sy->fr != NULL))
                                {
                                    forward_reference(t1.token_string, line_number, location_counter, TRUE);
                                }
                            
                        case Tconstant:
                            /* if we already have good stuff here, why do we have another constant
                               or symbol ? */
                            if (good_stuff)
                                {
                                    number_of_errors += 1;
                                    fprintf(stderr, "illegal token at line %d\n", line_number);
                                }

                            good_stuff = TRUE;
                            instruction = (t1.value & 0xFFF);
                            fixed_bits = 0xFFF;
                            get_token(&t1);
                            break;

                        default:
                            number_of_errors += 1;
                            fprintf(stderr, "illegal token at line %d\n", line_number);
                            t1.type = Tillegal;
                            break;
                        }
                }

            /* check if this line is just a comment */
            finish_this_line(location_counter, instruction);
            
            if (good_stuff)
                {
                    Define_Object_Code(location_counter, instruction, FALSE);
                    location_counter += 1;
                }
        }

    /* last little bit of allocated memory */
    if (t1.token_string != NULL)
        {
            free(t1.token_string);
            t1.token_string = NULL;
        }

}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void scanargs(STRING s)
{
    /* check each character of the option list for
       its meaning. */

    while (*++s != '\0')
        switch (*s)
            {

            case 'D': /* debug option */
                debug = TRUE;
                break;

            default:
                fprintf (stderr,"asm: Bad option %c\n", *s);
                fprintf (stderr,"usage: asm [-D] file\n");
                exit(1);
            }
}


char *change_file_name(STRING name, STRING old_ext, STRING new_ext)
{
    /* need enough space for the new extension */
    int n = strlen(name) + strlen(old_ext) + 1;
    
    /* make the copy */
    char *news = malloc(n);
    strcpy(news, name);

    /* can we find the old extension in this file ? */
    char *oldxs = strstr(news, old_ext);
    if (oldxs == NULL)
        {
            /* there is no old extension; add the new extension */
            news = strcat(news, new_ext);
        }
    else
        {
            while (*new_ext != '\0')
                *oldxs++ = *new_ext++;
            *oldxs = '\0';
            /* find the old extension in the old string */
            char *p = strstr(name, old_ext);
            /* and add the part after that to the new string */
            strcat(oldxs, &p[strlen(old_ext)]);
        }
        
    return(news);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int main(int argc, STRING *argv)
{

    Initialize_Opcode_Table();

    /* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments. */

    while (argc > 1)
        {
            argc--, argv++;
            if (**argv == '-')
                scanargs(*argv);
            else
                {
                    input = fopen(*argv,"r");
                    if (input == NULL)
                        {
                            fprintf (stderr, "Can't open %s\n",*argv);
                            continue;
                        }
                    char *out_filename = change_file_name(*argv, ".asm", ".obj");
                    pFile = fopen(out_filename,"w");
                    if (pFile == NULL)
                        {
                            fprintf (stderr, "Can't open %s\n",out_filename);
                            continue;
                        }

                    /* process the input assembly file */
                    Clear_Object_Code();
                    Assemble_File(*argv);
                    Check_for_undefined_symbols();
                    if (number_of_errors > 0)
                        fprintf(stderr, "*** %d errors in assembly\n", number_of_errors);

                    Output_Object_Code();

                    fclose(input);
                    fclose(pFile);
                    free(out_filename);
                }
        }

    exit(0);
}