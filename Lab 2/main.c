
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *input;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

Boolean debug = FALSE;

typedef char *STRING;

#define CAST(t,e) ((t)(e))
#define TYPED_MALLOC(t) CAST(t*, malloc(sizeof(t)))

void DoIt(STRING name);

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
                fprintf (stderr,"PROGR: Bad option %c\n", *s);
                fprintf (stderr,"usage: PROGR [-D] file\n");
                exit(1);
            }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int main(int argc, STRING *argv)
{
    Boolean filenamenotgiven = TRUE;

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
                    filenamenotgiven = FALSE;
                    input = fopen(*argv,"r");
                    if (input == NULL)
                        {
                            fprintf (stderr, "Can't open %s\n",*argv);
                        }
                    else
                        {
                            DoIt(*argv);
                            fclose(input);
                        }
                }
        }

    if (filenamenotgiven)
        {
            input = stdin;
            DoIt(NULL);
        }

    exit(0);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void DoIt(STRING name)
{
}
