/* 
   token processing for the assembler
   Take an input line, and break it up into tokens.
*/

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "opcode.h"
#include "symbol.h"

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Types for the token processing */

/* tokens, for parsing */
enum Token_type
{
    Tcolon,
    Topcode,
    Tsymbol,
    Tconstant,
    Tcomment,
    Tillegal
};

struct Token
{
    enum Token_type  type;
    char *token_string;
    opcode *op;
    symbol *sy;
    int    value;
};
typedef struct Token Token;

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* variables */
int line_number;

/* prototypes */
int get_next_line(void);
void finish_this_line(Address location_counter, INST instruction);
enum Token_type peek_token_type(void);
void get_token(Token *t);

#endif
