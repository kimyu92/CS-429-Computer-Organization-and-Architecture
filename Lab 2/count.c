
/* ***************************************************************** */

/*  count       histogram of byte values                             */

/* ***************************************************************** */

/*
  count -- count the number of occurrences of each byte in
  a file.

  James L. Peterson
  Department of Computer Sciences
  University of Texas, Austin

  Written and revised from 1980.   This program
  is a C version of an earlier Pascal version which was
  written and used on various systems, including CDC,
  IBM, DEC-10, and DEC-20.

  There are two options:

  -a  print all counts, even zero and controls.
  -z  print controls only if non-zero, print all
  printable characters even if counts are zero.

  (default)  print only non-zero counts.

  The input comes either from standard input or from a file
  name listed on the command line.
*/


#include <stdio.h>
#include <stdlib.h>

typedef short Boolean;
#define TRUE 1
#define FALSE 0

FILE * input;

Boolean z_option = FALSE /* print zeros */ ;
Boolean a_option = FALSE /* print all, even control codes */ ;
Boolean m_option = FALSE /* print mismatched pairs */;



struct cell
{
    long  count;
    long  firstline;
    long  lastline;
};

struct cell byte[256];



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

char *printwhere(struct cell *b)
{
    static char buffer[128];

    if ((b->firstline == -1) || (b->lastline == -1))
        return("");

    sprintf(buffer, "  line %ld-%ld", b->firstline, b->lastline);
    return(buffer);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

Boolean Open_Input_File(char *name)
{
    if (name == NULL)
        input = stdin;
    else
        {
            input = fopen(name, "r");
            if (input == NULL)
                {
                    fprintf(stderr, "Can't open %s\n", name);
                    return(FALSE);
                }
        }
    return(TRUE);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* statistics computed by Read_And_Count_Bytes() and printed by Print_Counts() */
long  NumberOfBytes = 0;
int   MAXlinelength = 0;
int   NumberOfLines = 1;
int   LongestLineNumber = 0;


void Read_And_Count_Bytes(void)
{
    int   c;
    int   CurrentLineLength = 0;

    for (c = 0; c < 256; c++)
        {
            byte[c].count = 0;
            byte[c].firstline = -1;
            byte[c].lastline = -1;
        }

    while ((c = getc(input)) != EOF)
        {
            NumberOfBytes += 1;

            byte[c].count += 1;
            byte[c].lastline = NumberOfLines;
            if (byte[c].firstline == -1)
                byte[c].firstline = NumberOfLines;

            if (c != '\n')
                CurrentLineLength += 1;
            else
                {
                    if (CurrentLineLength > MAXlinelength)
                        {
                            MAXlinelength = CurrentLineLength;
                            LongestLineNumber = NumberOfLines;
                        }
                    NumberOfLines += 1;
                    CurrentLineLength = 0;
                }
        };

    fclose(input);

    if (CurrentLineLength > MAXlinelength)
        {
            MAXlinelength = CurrentLineLength;
            LongestLineNumber = NumberOfLines;
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Print_Counts(char *name)
{
    int   c;

    printf("%s: Total bytes = %6ld \n", name, NumberOfBytes);
    printf("%s: Longest line = %5d bytes on line %4d of %d\n\n",
           name, MAXlinelength, LongestLineNumber, NumberOfLines);

    if (m_option)
        {
            if (byte['('].count != byte[')'].count)
                fprintf(stderr, "count '(' = %ld; count ')' = %ld\n",
                        byte['('].count, byte[')'].count );
            if (byte['{'].count != byte['}'].count)
                fprintf(stderr, "count '{' = %ld; count '}' = %ld\n",
                        byte['{'].count, byte['}'].count );
            if (byte['['].count != byte[']'].count)
                fprintf(stderr, "count '[' = %ld; count ']' = %ld\n",
                        byte['['].count, byte[']'].count );
        }

    for (c = 0; c < 32; c++)
        {
            int i = c + 64;
            if ((byte[c].count != 0) || a_option)
                printf("Hex 0x%02X Char ^%1c   Number %10ld  %s\n",
                       c, i, byte[c].count, printwhere(&(byte[c])));
        }

    for (c = 32; c < 127; c++)
        if ((byte[c].count != 0) || z_option)
            printf("Hex 0x%02X Char %1c    Number %10ld \n",
                   c, c, byte[c].count);

    if ((byte[127].count != 0) || z_option)
        printf("Hex 0x%02X Char Rub  Number %10ld  %s\n",
               127, byte[127].count, printwhere(&(byte[127])));

    for (c = 0 + 128; c < 32 + 128; c++)
        {
            int i = c + 64 - 128;
            if (byte[c].count != 0)
                printf("Hex 0x%02X Char +^%1c  Number %10ld  %s\n",
                       c, i, byte[c].count, printwhere(&(byte[c])));
        }

    for (c = 32 + 128; c < 127 + 128; c++)
        if (byte[c].count != 0)
            printf("Hex 0x%02X Char +%1c   Number %10ld  %s\n",
                   c, c - 128, byte[c].count, printwhere(&(byte[c])));

    if (byte[255].count != 0)
        printf("Hex 0x%02X Char +Rub Number %10ld  %s\n",
               255, byte[255].count, printwhere(&(byte[255])));

}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void process(char *name)
{
    if (!Open_Input_File(name)) return;

    Read_And_Count_Bytes();

    Print_Counts(name);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void scanargs(char *s)
{
    /* check each character of the option list for its meaning. */

    while (*s != '\0')
        switch (*s++)
            {
            case '-': /* option indicator */
                break;

            case 'a': /* print all; even controls */
                a_option = TRUE;
                break;

            case 'm': /* print mismatched pairs: {} and () */
                m_option = TRUE;
                break;

            case 'z': /* print zero counts */
                z_option = TRUE;
                break;

            default:
                fprintf(stderr, "count: Bad option %c\n", s[-1]);
                fprintf(stderr, "       -a -- print all counts\n");
                fprintf(stderr, "       -z -- print zero counts\n");
                fprintf(stderr, "       -m -- print mismatches\n");
                exit(1);
            }

    if (a_option)
        z_option = TRUE;
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int main(int   argc, char **argv)
{
    Boolean namenotgiven = TRUE;

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
                    namenotgiven = FALSE;
                    process(*argv);
                }
        }

    if (namenotgiven)
        {
            process(NULL);
        }

    exit(0);
}


