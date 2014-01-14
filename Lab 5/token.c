/* 
   token processing for the assembler
   Take an input line, and break it up into tokens.
*/


#include <ctype.h>
#include "asm8.h"
#include "token.h"

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

char *input_buffer = NULL;
int buffer_length = 0;
int input_line_length = 0;
int line_number;
int token_index = 0;

void save_char(char c)
{
    /* make sure we have room for this character in the input buffer */
    if (input_line_length >= buffer_length-1)
        {
            if (buffer_length == 0)
                buffer_length = 256;
            else
                buffer_length = 2*buffer_length;
            input_buffer = realloc(input_buffer, buffer_length);
        }
    input_buffer[input_line_length] = c;
    input_line_length += 1;
}

/* read the next line of input, and set the token pointer to the
   beginning of the line */
int get_next_line(void)
{
    int c;
    while (((c = getc(input)) != EOF) && (c != '\n'))
        {
            save_char(c);
        }
    save_char('\0');
    line_number += 1;
    token_index = 0;
    
    if ((c == EOF) || (input_line_length == 0))
        return(EOF);
    else
        return(input_line_length);
}

/* print out this line and it's contents (if there are any) */
void finish_this_line(Address location_counter, INST instruction)
{
    fprintf(stdout, "  ");
    if (good_stuff)
        {
            fprintf(stdout, "0x%03X: 0x%03X", location_counter, instruction);
        }
    else
        {
            fprintf(stdout, "            ");
        }
    fprintf(stdout, "%5d: %s\n", line_number, input_buffer);

    input_line_length = 0;
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

char *copy_token_string(int i, int j)
{
    /* change j to '\0', so we can copy the string */
    char save = input_buffer[j+1];
    input_buffer[j+1]= '\0';
    char *s = remember_string(&input_buffer[i]);
    /* restore the input string, and update to just beyond this token */
    input_buffer[j+1] = save;
    return(s);
}

/* look at the next token to see what it will be */
/* only need to look one character ahead */
enum Token_type peek_token_type(void)
{
    /* skip any leading spaces */
    while( (token_index < input_line_length) && 
           (isspace(input_buffer[token_index]))  ){
            token_index += 1;
    }


    /* check for an empty line */
    if (token_index >= input_line_length) {
        return(Tillegal);
    }
    

    if (input_buffer[token_index] == '\0'){
        return(Tillegal);
    }

    /* colons are for labels */
    if (input_buffer[token_index] == ','){
        return(Tcolon);
    }

    /* check for comments */
    if ((input_buffer[token_index] == '/')){
        return(Tcomment);
    }

    /* by symbol, we mean symbol or number */
    return(Tsymbol);
}



/* check for constants of different bases: 8, 10, 16 */
Boolean is_valid_digit(char c, int base)
{
    switch(base)
        {
        case 16:
            if (('A' <= c) && (c <= 'F')) return(TRUE);
            if (('a' <= c) && (c <= 'f')) return(TRUE);
        case 10:
            if (('8' <= c) && (c <= '9')) return(TRUE);
        case 8:
            if (('0' <= c) && (c <= '7')) return(TRUE);
        }
    return(FALSE);
}

int digit_value(char c, int base)
{
    switch(base)
        {
        case 16:
            if (('A' <= c) && (c <= 'F')) return(10+c-'A');
            if (('a' <= c) && (c <= 'f')) return(10+c-'a');
        case 10:
            if (('8' <= c) && (c <= '9')) return(c-'0');
        case 8:
            if (('0' <= c) && (c <= '7')) return(c-'0');
        }
    return(-1);
}



/* read a constant from the input buffer */
Boolean parse_constant(Token *t, int *value)
{
    /* remember where it starts */
    int b0 = token_index;
    int base = 10;
    int n = 0;
    int sign = 1;

    /* negative numbers */
    if (input_buffer[token_index] == '-')
        {
            sign = -1;
            token_index += 1;
        }

    /* base is decimal unless ... */
    if (input_buffer[token_index] == '0')
        {
            /* 0...  -- octal */
            token_index += 1;
            base = 8;
            if (input_buffer[token_index] == 'x')
                {
                    /* 0x...  -- hex */
                    token_index += 1;
                    base = 16;
                }
        }
    while (is_valid_digit(input_buffer[token_index], base))
        {
            n = n * base + digit_value(input_buffer[token_index], base);
            token_index += 1;            
        }
    *value = sign * n;

    t->token_string = copy_token_string(b0, token_index);    

    if (isalpha(input_buffer[token_index]))
        return(FALSE);
    else
        return(TRUE);
}

void get_token(Token *t)
{
    /* check if we need to free the previous string */
    if (t->token_string != NULL)
        {
            free(t->token_string);
            t->token_string = NULL;
        }

    enum Token_type a = peek_token_type();
    if (a != Tsymbol)
        {
            t->type = a;
            t->token_string = copy_token_string(token_index, token_index);
            token_index += 1;
            if (debug)
                fprintf(stderr, "next token: %s\n", t->token_string);
            return;
        }

    if (isalpha(input_buffer[token_index]))
        {
            int j;
            for (j = token_index; j < input_line_length; j++)
                {
                    if (isalpha(input_buffer[j])) continue;
                    if (isdigit(input_buffer[j])) continue;
                    if (input_buffer[j] == '_') continue;
                    if (input_buffer[j] == '.') continue;
                    /* not alpha, digit or underbar or period */
                    break;
                }
            /* symbol is from token_index to j-1 */
            t->token_string = copy_token_string(token_index, j-1);
            token_index = j;
            if (debug) fprintf(stderr, "next token: %s\n", t->token_string);

            /* search to see if this token is an opcode */
            opcode *op = search_opcode(t->token_string);
            if (op != NULL)
                {
                    t->type = Topcode;
                    t->op = op;
                    return;
                }

            /* search to see if this token is a known symbol */
            symbol *sy = search_symbol(t->token_string);
            if (sy != NULL)
                {
                    t->type = Tsymbol;
                    t->sy = sy;
                    t->value = sy->value;
                    return;
                }

            /* we have a symbol that is not currently defined */
            t->type = Tsymbol;
            t->sy = NULL;
            t->value = 0;
            return;
        }

    /* A numeric constant */
    else if (isdigit(input_buffer[token_index]) || (input_buffer[token_index] == '-'))
        {
            int value;
            if (parse_constant(t, &value))
                {
                    t->type = Tconstant;
                    t->value = value;
                    if (debug) fprintf(stderr, "next token: constant %d\n", t->value);
                    return;
                }
        }

    /* A character constant: '.'  */
    else if ((input_buffer[token_index] == '\'') && (input_buffer[token_index+2] == '\''))
        {
            t->type = Tconstant;
            t->value = input_buffer[token_index+1];
            t->token_string = malloc(4);
            t->token_string[0] = t->token_string[2] = '\'';
            t->token_string[1] = t->value;
            t->token_string[3] = '\0';
            token_index += 3;
            if (debug) fprintf(stderr, "next token: char constant %d (%c)\n", t->value, t->value);
            return;
        }

    /* and anything else is just illegal */
    /* should we advance the token_index here? */
    t->type = Tillegal;
    if (debug) fprintf(stderr, "next token: illegal\n");
    return;
}

