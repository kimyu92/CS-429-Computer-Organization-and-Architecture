/*
  PDP-8 Assembler:

  input is an assembly language program.  Output is an object file.
*/

#ifndef _ASM8_H_
#define _ASM8_H_

/* standard environment for our coding */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef short Boolean;
#define TRUE 1
#define FALSE 0


typedef char *STRING;

#define CAST(t,e) ((t)(e))
#define TYPED_MALLOC(t) CAST(t*, malloc(sizeof(t)))

Boolean debug;

FILE *input;
FILE *output;


STRING remember_string(const STRING name);

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Types specific for the assembler */

/* address type */
typedef short Address;

/* ***************************************************************** */
/* opcodes */

typedef short INST;


/* ***************************************************************** */
/* error handling */

int number_of_errors;


/* The assembled instruction.  Plus do we actually have 
   anything (good_stuff) or is there no output. */
Boolean good_stuff;

/* ***************************************************************** */
/* prototypes */
INST Adjust_for_ZC(Address pc, INST instruction, Address addr);


#endif
