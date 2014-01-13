//Name   : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA     : Ji Hong
 
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
 
FILE *input;
FILE *input2;
 
typedef short Boolean;
#define TRUE 1
#define FALSE 0
 
typedef char *String;
 
char *arrStr;
char *res;
char *fact;


int obj_index = 0;
int prop_index = 1;
int value_index = 2;
 
//Functions scope
void process();

char *somehow_get_a_line();
char* deblank(char* input);

void get_a_fact(String thisString);
char *substr(const char *data_str, int pos_start, int pos_end);

void create_the_fact_array();
void resize_the_fact_array();
//================================
//End of Declaration
//================================
 
 
int main(int   argc, char **argv){
    Boolean hasFile = FALSE;
    String extension = strrchr(*argv, '.');
    /* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments.
    */
 
    while (argc > 1 && !hasFile){
 
        argc--,
        argv++;
 
        if (!extension){
            printf ("Please feed in .fax file\n");
            hasFile == FALSE;
        }
        else{
            printf("Thanks for feeding in with the extension of %s\n", extension + 1);
             
            input = fopen(*argv,"r");
 
            argc--;
            argv++;
 
            input2 = fopen(*argv,"r");
             
 
            if(input2 != NULL){
                //Check the file extension
                if ( (strrchr(*argv, '.') + 1)[0]== 'q' ){
                    hasFile = TRUE;
                    printf("This is the q file\n");
 
                    process();
 
 
                    fclose(input);
                    fclose(input2);
                }
                 
 
                 
            }
            else
                printf("Please feed me the stdin/ second file\n\n");
             
 
        }
 
 
 
         
    }
 
 
    exit(0);
}


//Main Process
void process(){
    //Get a line
    somehow_get_a_line();

    //From a line start to process
    //get_a_fact();
}


//Getting a string
char *somehow_get_a_line(){
    int aByte;
 
    Boolean theStartIsF = FALSE;
    Boolean countedTheLine = TRUE;
     
    int charInLine = 0;
    int maxCharInLine = 0;

 
    
 
    while ( (aByte = fgetc(input)) != EOF){
 
        //Switch on before a line
        if(aByte == 'F')
            theStartIsF = TRUE;
 
        //Count for the maximum line
        if(theStartIsF && aByte != '\n')
            charInLine++;
 
        //Switch of the if statement down there
        if(aByte == ':')
            countedTheLine = FALSE;

        //Switch off after a line
        if(aByte == '\n'){
            theStartIsF = FALSE;
 
            //Find the longest length amongs the line
            if(charInLine > maxCharInLine)
                maxCharInLine = charInLine;
 
            charInLine = 0;
 
        }
 
         
    }
 
    //Go back to the head
    rewind(input);
    printf("This is whatever size %d\n", maxCharInLine);
    printf("============================\n\n");
 
    //Allocate memory for fgets
    arrStr = (char*) malloc( (sizeof(char)) * (maxCharInLine + 1) );
    free(arrStr);

     
    //Go again my size
    while ( (fgets(arrStr, (maxCharInLine + 1), input)) != NULL ){

        //Skip a blank line
        if(arrStr[0] != '\n' ){
            printf("This is before     : %s\n", arrStr);                           //Debugging

            //Cutting down the string
            arrStr = deblank(arrStr);
        
            printf("What is the truncated: %s\n", arrStr);                         //Debugging

            //Getting the fact
            get_a_fact(arrStr);
        }

    }


}


//Cutdown the string
char* deblank(char* input){
    char *output = input;
    int i = 0;
    int j = 0;
    
    while(i < strlen(input)){
        if (input[i]!=' ')                           
            output[j]=input[i];                     
        else
            j--;

        i++;
        j++;
    }                           
    

    output[j] = 0;

    return output;
}




void get_a_fact(String thisString){
    char *s = thisString;
    int len = strlen(s);

    int pos_colon = -1;
    int pos_equ = -1;
    
    int i;

    char *object_name, *property_name, *value_name;


    if (s[0] != 'F')
        return;


    for (i = 0; i < len; i++){
        if (s[i] == ':'){
            pos_colon = i;
            break;
        }
    }
     
    for (i = 0; i < len; i++){
        if (s[i] == '='){
            pos_equ = i;
            break;
        }
    }
 
    if (pos_colon == -1 || pos_equ == -1){
        return;
    }

    //Instantiate array
    create_the_fact_array();

    

    // printf("%d\n", pos_colon);
    // printf("%d\n", pos_equ);
    printf("Result: \n");                               //Debugging starts

    

    //Grabbing each element
    object_name = substr(s, 1, pos_colon );
    printf("%s\n", object_name);
    
    //fact[obj_index] = object_name;
    obj_index++;

    free(res);

    property_name = substr(s, pos_colon + 1, pos_equ);
    printf("%s\n", property_name);

    //fact[prop_index] = object_name;
    prop_index++;

    free(res);

    value_name = substr(s, pos_equ + 1, len);
    printf("%s\n\n", value_name);

    //fact[prop_index] = object_name;
    prop_index++;

    free(res);
}


//Helper for get_a_fact
//Fetch the data/value
char *substr(const char *data_str, int pos_start, int pos_end){
    int amount = pos_end - pos_start;
    
    //printf("HERE i am   \n");
    //char *res = (char*) malloc( (sizeof(char)) * (amount + 1) );
    res = (char*) malloc( (sizeof(char)) * (amount + 1) );
    //printf("HERE i am v2  \n");

    //printf("HERE i am v3  \n");
    memcpy(res, data_str + pos_start, amount);
    //printf("HERE i am v4  \n");


    return res;
}

void create_the_fact_array(){
    int obj_size = 10, prop_size = 10, value_size = 1;

    //Contagious 3d array
    fact = (char *) malloc(obj_size * prop_size * value_size * (sizeof(char)) );
    free(fact);
}


void resize_the_fact_array(){




}