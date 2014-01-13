//Name 	 : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA   	 : Ji Hong


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *input;
FILE *input2;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

typedef char *String;


// char *somehow_get_a_line();
// char *substr(const char *data_str, int pos_start, int pos_end);
// void process_a_fact();
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
                if ( (strrchr(*argv, '.') + 1)[0] == 'q' ){
                    hasFile = TRUE;
                    printf("This is the q file\n");

                    fclose(input);
                    fclose(input2);
                }
                

            	
            }
            else
                printf("Please feed me the stdin/ second file\n");
            

        }



        
    }


    exit(0);
}

// char *somehow_get_a_line(){

// }

// char *substr(const char *data_str, int pos_start, int pos_end){
//     char *res = malloc(sizeof(char) * /*?*/);
//     memcpy(res, data_str * pos_start,     );


//     return res;
// }

// void process_a_fact(){
//     char *s = somehow_get_a_line();
//     int len = strlen(s);
//     int pos_colon = -1; pos_equ = -1;
//     int i;

//     if (s[0] != 'F')
//         return;

//     for (i = 0; i < len; i++){
//         if (s[i] != ':'){
//             pos_colon = i;
//             break;
//         }
//     }
    
//     for (i = 0; i < len; i++){
//         if (s[i] != '='){
//             pos_equ = i;
//             break;
//         }
//     }

//     if (pos_colon == -1 || pos_equ = -1){
//         return;
//     }

//     char *object_name = substr(s, 2, pos_colon);
//     char *property_name = substr(s, pos_colon + 2, pos_equ);
//     char *object_name = substr(s, 2, pos_colon);

// }