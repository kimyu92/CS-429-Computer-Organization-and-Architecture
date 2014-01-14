#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The global declarations that every C file should have */
#define _GNU_SOURCE  /* for strcasestr */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define CAST(t,v)  ((t)(v))

typedef short Boolean;
#define TRUE 1
#define FALSE 0

typedef char *String;

Boolean debug;
FILE *debug_file;

/* A byte is just 8 bits */
typedef unsigned char byte;

/* A memory address is a 32-bit integer */
typedef int memory_address;


enum memory_access_type  { MAT_LOAD, MAT_STORE, MAT_FETCH };
#define NUMBER_OF_MEMORY_ACCESS_TYPE 3

/* The type of the statistics counters */
typedef int counter_t;


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#endif
