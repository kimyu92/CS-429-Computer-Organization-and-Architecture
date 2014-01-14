
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The code to read a memory trace and simulate it on the various
   caches. */

#include "global.h"
#include "utils.h"        // utility functions


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* n is a power of two, which one? */
/* 1 -> 0; 2 -> 1; ... */
int which_power(int n)
{
    int i = 0;
    int t = 1;
    while (n > t)
        {
            t += t;
            i += 1;
        }
    return(i);
}

/* generate a mask of n low order bits */
/* if we want a mask of two bits, shift a 1 two
   bits over (100), and then subtract one (011). */
unsigned int mask_of(int n)
{
    int m;
    if (n <= 0) return(0);
    m = (1 << n) - 1;
    return(m);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

Boolean ishex(int c)
{
    if (('0' <= c) && (c <= '9')) return(TRUE);
    if (('a' <= c) && (c <= 'f')) return(TRUE);
    if (('A' <= c) && (c <= 'F')) return(TRUE);
    return(FALSE);
}

int hexvalue(int c)
{
    if (('0' <= c) && (c <= '9')) return(c - '0');
    if (('a' <= c) && (c <= 'f')) return(c - 'a' + 10);
    if (('A' <= c) && (c <= 'F')) return(c - 'A' + 10);
    return(-1);
}

int decvalue(int c)
{
    if (('0' <= c) && (c <= '9')) return(c - '0');
    return(-1);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* MALLOC space for a string and copy it */

String remember_string(const String name)
{
    size_t n;
    String p;

    if (name == NULL) return(NULL);

    /* get memory to remember file name */
    n = strlen(name) + 1;
    p = CAST(String , malloc(n));
    strcpy(p, name);
    return(p);
}

String augment_name(const String name, const String plus)
{
    int n = strlen(name) + 1 + strlen(plus) + 1;
    String p = CAST(String, malloc(n));
    sprintf(p, "%s %s", name, plus);
    return(p);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int skip_blanks(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && isspace(c)) /* keep reading */;
    return(c);
}


int skip_line(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && (c != '\n')) /* keep reading */;
    c = skip_blanks(file);
    return(c);
}


