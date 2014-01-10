/* ***************************************************************** */


/*  hex convert file to ascii hex */

/*  author  James Peterson */

/*
  hex: to convert the input file to a sequence of hexadecimal
  character code characters.  This allows you to get at each
  and every possible character.

  modified to allow translation to hex, octal, base 4 or binary.

  Written around  1980.

*/

/* ***************************************************************** */


/* missing type from C language */
typedef short Boolean;
#define TRUE 1
#define FALSE 0


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <stdio.h>
#include <stdlib.h>

FILE * input;

short   base = 16 /* base of the translation */ ;
short   basebits = 4 /* number of bits per digit to shift */ ;

short   DesiredLineLength = 0;

void process(void);

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void scanargs(char   *s)
{
    short   n;

    s++ /* skip minus sign */ ;

    switch (*s)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':          /* numeric option is base of translation */
            n = atoi(s);
            if (n == 16)
                {
                    base = 16;
                    basebits = 4;
                }
            else if (n == 8)
                {
                    base = 8;
                    basebits = 3;
                }
            else if (n == 4)
                {
                    base = 4;
                    basebits = 2;
                }
            else if (n == 2)
                {
                    base = 2;
                    basebits = 1;
                }
            else
                {
                    fprintf(stderr, "base must be 16, 8, 4, or 2\n");
                    base = n;
                }
            break;
            
        case 'w':          /* line width */
            n = atoi(++s);
            if (n > 2)
                DesiredLineLength = n;
            break;

        default:
            fprintf(stderr, "hex: Bad option %c\n", *s);
            fprintf(stderr, "usage: hex [-base][-wn] file\n");
            exit(1);
        }
}

/* ***************************************************************** */

int main(int argc, char  **argv)
{
    Boolean namenotgiven = TRUE;

    /* main driver program.  Define the input file from either standard input or
       a name on the command line.  Process all arguments. */

    while (argc > 1)
        {
            argc--, argv++;
            if (**argv == '-')
                scanargs(*argv);
            else
                {
                    namenotgiven = FALSE;
                    input = fopen(*argv, "r");
                    if (input == NULL)
                        {
                            fprintf(stderr, "Can't open %s\n", *argv);
                            exit(1);
                        }
                    process();
                    fclose(input);
                }
        }

    /* input from stdin if not file named on the input line */
    if (namenotgiven)
        {
            input = stdin;
            process();
        }

    exit(0);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void process(void)
{
    int    c;
    short  CurrentLineLength;
    short  basedigits = 2 /* number of digits per byte */ ;
    char   d[17] /* for the digits */ ;
    int    i /* index through digits */ ;

    if (DesiredLineLength < 2)
        DesiredLineLength = 64;
    CurrentLineLength = 0;

    basedigits = (8 + basebits - 1) / basebits;

    while ((c = getc(input)) != EOF)
        {
            if (base == 10)
                {
                    fprintf(stdout, "%3d ", c);
                    CurrentLineLength += 4;
                }
            else
                {
                    for (i = 1; i <= basedigits; i++)
                        {
                            /* convert each byte to string of digits */
                            d[i] = (c & (base - 1));
                            d[i] += (d[i] <= 9) ? '0' : 'A' - 10;
                            c = c >> basebits;
                        }

                    /* and output the digits */
                    for (i = basedigits; 1 <= i; i--)
                        {
                            putc(d[i], stdout);

                            /* increment line length; check for end of line */
                            CurrentLineLength += 1;
                        }
                }

            if (CurrentLineLength >= DesiredLineLength)
                {
                    putc('\n', stdout);
                    CurrentLineLength = 0;
                }
        }

    /* be sure to end any partial output lines */
    if (CurrentLineLength > 0)
        putc('\n', stdout);
}

