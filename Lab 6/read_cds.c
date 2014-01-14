
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Reading cache descriptions */

#include "global.h"
#include "utils.h"
#include "cds.h"
#include "caches.h"

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we want to read in a string of some unknown length */
#define MIN_TOKEN_SIZE 128

struct Token
{
    String string;
    int    length;
};
typedef struct Token Token;

Token *new_token(void)
{
    Token *t = CAST(Token *, malloc(sizeof(Token)));
    t->length = MIN_TOKEN_SIZE;
    t->string = CAST(String, malloc(t->length));
    return(t);
}

void delete_token(Token *t)
{
    free(t->string);
    free(t);
}

void put_char_in_token_at(Token *t, char c, int i)
{
    if (i >= t->length)
        {
            /* need more space */
            t->length = 2 * t->length;
            t->string = CAST(char *,realloc(t->string, t->length));
            if (t->string == NULL)
                {
                    fprintf(stderr, "Hell has frozen over!!!\n");
                    exit(-1);
                }
        }
    t->string[i] = c;
}


void get_token(FILE *CDS_file, Token *t)
{
    int c;

    /* get the next token in the input stream */
    int i = 0;

    /* token is empty to start */
    put_char_in_token_at(t, '\0', i);

    /* skip spacing, look for first character */
    c = skip_blanks(CDS_file);
    if (c == EOF) return;

    while (isalnum(c) || (c == '_'))
        {
            put_char_in_token_at(t, c, i);
            i = i + 1;
            put_char_in_token_at(t, '\0', i);
            c = getc(CDS_file);
        }

    /* went one too far, put it back */
    ungetc(c, CDS_file);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Syntax for Cache Descriptions:  { key=value, key=value, ... } */
/* So, we read a key and a value and define the field of the
   cds defined by the key to have the given value. */

int get_key_value_pair(FILE *CDS_file, Token *key, Token *value)
{
    int c;

    /* skip initial spaces */
    c = skip_blanks(CDS_file);
    if (c == EOF) return(EOF);
    if (c == '}') return(EOF);

    /* went one too far, put it back */
    ungetc(c, CDS_file);

    /* we want a string for the key */
    get_token(CDS_file, key);

    /* skip spacing, look for "=" */
    c = skip_blanks(CDS_file);
    if (c == EOF) return(EOF);
    if ((c != '=') && (c != ':') && (c != '-'))
        {
            fprintf(stderr, "not key=value pair: %s %c\n", key->string, c);
            return(EOF);
        }

    /* we want a second string for the value */
    get_token(CDS_file, value);

    /* skip spacing, look for "," */
    c = skip_blanks(CDS_file);
    if (c == EOF) return(EOF);
    if ((c != ',') && (c != ';') && (c != '}'))
        {
            fprintf(stderr, "not key=value pair: %s %c\n", key->string, c);
            return(EOF);
        }
    if (c == '}')
        {
            /* we have the last pair, terminated by a '}'.
               put it back, so that this last pair is processed */
            ungetc(c, CDS_file);
            return(',');
        }

    return(c);
}


void define_key_value_pair(CDS *cds, Token *key, Token *value)
{
    if (debug) fprintf(debug_file, "define %s = %s \n", key->string, value->string);

    /* look for the name */
    if (strcasestr(key->string, "name") != NULL)
        {
            if (cds->name != NULL) free(cds->name);
            cds->name = remember_string(value->string);
            return;
        }

    /* look for line size */
    if ((strcasestr(key->string, "line") != NULL) && (strcasestr(key->string, "size") != NULL))
        {
            int n = atoi(value->string);
            cds->c->cache_line_size = n;
            cds->v->cache_line_size = n;    //grab number of bytes per victim cache line
            return;
        }

    /* look for number of cache entries */
    if (strcasestr(key->string, "entries") != NULL)
        {
            int n = atoi(value->string);
            cds->c->number_of_cache_entries = n;
            return;
        }

    /* look for the number of ways */
    if (strcasestr(key->string, "ways") != NULL)
        {
            int n = atoi(value->string);
            cds->c->number_of_ways = n;
            return;
        }

    /* look for write-back */
    if ((strcasestr(key->string, "write") != NULL) && (strcasestr(key->string, "back") != NULL))
        {
            if (strcasestr(value->string, "true") != NULL)
                {
                    cds->c->write_back = TRUE;
                    return;
                }
            if (strcasestr(value->string, "false") != NULL)
                {
                    cds->c->write_back = FALSE;
                    return;
                }
        }

    /* look for write-thru */
    if ((strcasestr(key->string, "write") != NULL) && (strcasestr(key->string, "thru") != NULL))
        {
            if (strcasestr(value->string, "true") != NULL)
                {
                    cds->c->write_back = FALSE;
                    return;
                }
            if (strcasestr(value->string, "false") != NULL)
                {
                    cds->c->write_back = TRUE;
                    return;
                }
        }

    /* look for the replacement policy */
    if ((strcasestr(key->string, "policy") != NULL) || (strcasestr(key->string, "replace") != NULL))
        {
            if (strcasestr(value->string, "LRU") != NULL)
                {
                    cds->c->replacement_policy = CRP_LRU;
                    return;
                }
            if (strcasestr(value->string, "LFU") != NULL)
                {
                    cds->c->replacement_policy = CRP_LFU;
                    return;
                }
            if (strcasestr(value->string, "random") != NULL)
                {
                    cds->c->replacement_policy = CRP_RANDOM;
                    return;
                }
            if (strcasestr(value->string, "FIFO") != NULL)
                {
                    cds->c->replacement_policy = CRP_FIFO;
                    return;
                }
        }

    //Look for victim cache
    if (strcasestr(key->string, "victim") != NULL){
        cds->v->number_of_cache_entries = atoi(value->string);  //grab the number of entries    
        cds->v->replacement_policy = CRP_FIFO;                  //set it to FIFO
        return;
    }

    /* look for line size */
    if ((strcasestr(key->string, "decay") != NULL) && (strcasestr(key->string, "interval") != NULL))
        {
            int n = atoi(value->string);
            cds->c->LFU_Decay_Interval = n;
            return;
        }

    fprintf(stderr, "don't understand %s = %s\n",key->string, value->string);
} 


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

CDS *Read_CDS_file_entry(FILE *CDS_file)
{
    int c;

    c = skip_blanks(CDS_file);
    while (c == '#')
        {
            c = skip_line(CDS_file);
        }
    if (c == EOF) return(NULL);

    /* Syntax for Cache Descriptions:  { key=value, key=value, ... } */
    /* So, we read a key and a value and define the field of the
       cds defined by the key to have the given value. */

    if (c != '{')
        {
            fprintf(stderr, "Cache description should start with {, not %c\n", c);
            return(NULL);
        }

    /* starting a new cache description.  Get a structure,
       and fill in default values. */
    CDS *cds = CAST(CDS *,calloc(1,sizeof(CDS)));
    cds->name = remember_string("dummy");
    cds->c = CAST(struct cache *,calloc(1,sizeof(struct cache)));

    /* default values */
    cds->c->cache_line_size = 64;
    cds->c->number_of_cache_entries = 1024;
    cds->c->number_of_ways = 2;
    cds->c->write_back = TRUE;
    cds->c->replacement_policy = CRP_FIFO;
    cds->c->LFU_Decay_Interval = 200000;
    cds->c->c_line = NULL;

    Token *key = new_token();
    Token *value = new_token();
    while (((c = get_key_value_pair(CDS_file, key, value)) != EOF) && (c != '}'))
        {
            define_key_value_pair(cds, key, value);
        }
    delete_token(key);
    delete_token(value);

    cds->c->name = remember_string(cds->name);

    if (debug) debug_print_cds(cds);
    
    return(cds);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Read_Cache_Descriptions(String CDS_file_name)
{
    FILE *CDS_file;
    CDS *cds;

    /* open input file */
    CDS_file = fopen(CDS_file_name, "r");
    if (CDS_file == NULL)
        {
            fprintf (stderr,"Cannot open CDS file %s\n", CDS_file_name);
        }
    while ((cds = Read_CDS_file_entry(CDS_file)) != NULL)
        {
            /* we use a linked list for all the cache descriptions,
               but we want to keep the list in the same order that
               we read them in.  Bummer. */
            if (CDS_root == NULL)
                {
                    CDS_root = cds;
                }
            else
                {
                    CDS *q = CDS_root;
                    while (q->next != NULL) q = q->next;
                    q->next = cds;
                }
            cds->next = NULL;
        }
    fclose(CDS_file);
}
