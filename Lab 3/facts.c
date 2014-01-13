//Name   : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA     : Ji Hong
 
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
FILE *input;
FILE *input2;
 
typedef short Boolean;
#define TRUE 1
#define FALSE 0
 
typedef char *String;


//Functions scope
void process();

//Grab String
String somehow_process_q_line();
String somehow_get_a_line();
String deblank(String input);
String de_backslah_n(String cutphrase);

//Get fact and store them
void get_the_question(String thisString);
void get_a_fact(String thisString);

//Update the value
void update_value(String object_name, String property_name, String value_name);
//==================================
//End of Declaration without struct
//==================================


//Struct of ListNode
typedef struct ListNode{
    String object_data;
    String property_data;
    String value_data;  
    
    //The next node
    struct ListNode *next;
}ListNode;

//Memory-related variables
ListNode *root = NULL;
//ListNode *temp = NULL;

//For the get_the_question or get_a_fact methods


//For the deblank
char *output;


//Method prototype from strct
void display(ListNode *r);
void addNode(ListNode *adding);

void result(ListNode *r);
void freeMemNodes(ListNode *ptr);
void freeList(ListNode *head);

//==================================
//End of Declaration of struct
//==================================



int main(int argc, char **argv){
    Boolean hasFile = FALSE;
    String extension = strrchr(*argv, '.');
    /* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments.
    */

    while (argc > 1 && !hasFile){

        if (!extension){
            //printf ("Please feed in .fax file\n");
            hasFile == FALSE;
        }
        else{
            //printf("Thanks for feeding in with the extension of %s\n", extension + 1);
             
            input = fopen(*argv,"r");
 
            argc--;
            argv++;
 
            input2 = fopen(*argv,"r");
            //printf(" yeaa  \n");
 
            if(input2 != NULL){
                //Check the file extension
                if ( (strrchr(*argv, '.') + 1)[0]== 'q' ){
                    hasFile = TRUE;
                    //printf("This is the q file\n");
 
                    process();
 
                    fclose(input);
                    fclose(input2);
                }
                 
 
                 
            }
            else{
                //printf("Please feed me the stdin/ second file\n\n");
                input2 = stdin;
            }
        }

    }
 
 
    exit(0);
}

void result(ListNode *r){
    if(r == NULL)
        return;
    
    while(r != NULL){
        printf("F %s:",r->object_data);
        printf(" %s=",r->property_data);
        printf("%s\n",r->value_data);
        r=r->next;

    }
}

//Main Process
void process(){
    //Get a line
    somehow_process_q_line();
    somehow_get_a_line();
    result(root);
}


void freeList(ListNode *head){
   ListNode *tmp;

   while (head != NULL){
       tmp = head;
       head = head->next;

       free(tmp);
    }
}

String somehow_process_q_line(){
    int aByte;
 
    Boolean theStartIsQ = FALSE;
    Boolean countedTheLine = TRUE;
     
    int charInLine = 0;
    int maxCharInLine = 0;

    //Read byte by byte to get the length of longest line 
    while ( (aByte = fgetc(input2)) != EOF){
 
        //Switch on before a line
        if(aByte == 'Q')
            theStartIsQ = TRUE;
 
        //Count for the maximum line
        if(theStartIsQ && aByte != '\n')
            charInLine++;
 
        //Switch of the if statement down there
        if(aByte == ':')
            countedTheLine = FALSE;

        //Switch off after a line
        if(aByte == '\n'){
            theStartIsQ = FALSE;
 
            //Find the longest length amongs the line
            if(charInLine > maxCharInLine)
                maxCharInLine = charInLine;
 
            charInLine = 0;
 
        }    
    }

    if(maxCharInLine == 0 && (charInLine != 0) )
        maxCharInLine = charInLine;


    //Go back to the head
    rewind(input2);
 
    //Allocate memory for fgets
    char *arrStr = (char*) malloc( (sizeof(char)) * (maxCharInLine + 2) );
     
    //Go again my size
    while( (fgets(arrStr, (maxCharInLine + 2), input2)) != NULL ){

        //Skip a blank line
        if(arrStr[0] != '\n' ){

            //Cutting down the string
            arrStr = deblank(arrStr);
        
            //Getting the fact
            get_the_question(arrStr);
        }

    }
    free(arrStr);
    arrStr = NULL;
}



void get_the_question(String s){
    ListNode *temp = (ListNode*)malloc(sizeof(ListNode));
    int len = strlen(s);
    int pos_colon = -1;
    
    int i;
    int j;

    int amount;


    if (s[0] != 'Q')
        return;

    for (i = 0; i < len; i++)
        if (s[i] == ':'){
            pos_colon = i;
            break;
        }
    

    if (pos_colon == -1)
        return;

    //Grabbing each element
    temp->object_data = (char*) malloc( (sizeof(char)) * (pos_colon) );
    amount = pos_colon - 1;
    strncpy(temp->object_data, s + 1, amount);
    temp->object_data = de_backslah_n(temp->object_data);

    

    temp->property_data = (char*) malloc( (sizeof(char)) * (len - pos_colon) );
    amount = len - (pos_colon + 1);
    strncpy(temp->property_data, s + (pos_colon + 1), amount);
    temp->property_data = de_backslah_n(temp->property_data);


    temp->value_data = (char*) malloc( (sizeof(char)) * (8) );
    strcpy(temp->value_data, "unknown"); 


    addNode(temp);

    free(temp->object_data);
    free(temp->property_data);
    free(temp->value_data);
    free(temp);

}


//Getting a string
String somehow_get_a_line(){
    int aByte;
 
    Boolean theStartIsF = FALSE;
    Boolean countedTheLine = TRUE;
     
    int charInLine = 0;
    int maxCharInLine = 0;


    //Read byte by byte to get the length of longest line 
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

    if(maxCharInLine == 0 && (charInLine != 0) )
        maxCharInLine = charInLine;
 
    //Go back to the head
    rewind(input);

 
    //Allocate memory for fgets
    char *arrStr = (char*) malloc( (sizeof(char)) * (maxCharInLine + 2) );
     
    //Go again my size
    while ( (fgets(arrStr, (maxCharInLine + 2), input)) != NULL ){

        //Skip a blank line
        if(arrStr[0] != '\n' ){

            //Cutting down the string
            arrStr = deblank(arrStr);

            //Getting the fact
            get_a_fact(arrStr);
        }

    }

    free(arrStr);
    arrStr = NULL;
}



//Cutdown the string
String deblank(char* input){
    //char *output = input;
    int i = 0;
    int j = 0;

    output = input;
    
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



void get_a_fact(String s){
    String object_name;
    String property_name;
    String value_name;

    ListNode *temptemp; 

    int len = strlen(s);

    int pos_colon = -1;
    int pos_equ = -1;
    
    int i;
    int j;
    int amount;

    if (s[0] != 'F')
        return;

    for (i = 0; i < len; i++)
        if (s[i] == ':'){
            pos_colon = i;
            break;
        }
    
     
    for (i = 0; i < len; i++)
        if (s[i] == '='){
            pos_equ = i;
            break;
        }
    
 
    if (pos_colon == -1 || pos_equ == -1)
        return;


    //Grabbing each element
    object_name = (char*) malloc( (sizeof(char)) * (pos_colon) );
    amount = pos_colon - 1;
    strncpy(object_name, s + 1, amount);
    object_name = de_backslah_n(object_name);


    property_name = (char*) malloc( (sizeof(char)) * (pos_equ - pos_colon) );
    amount = pos_equ - (pos_colon + 1);
    strncpy(property_name, s + (pos_colon + 1), amount);
    property_name = de_backslah_n(property_name);


    value_name = (char*) malloc( (sizeof(char)) * (len - pos_equ) );
    amount = len - (pos_equ + 1);
    strncpy(value_name, s + (pos_equ + 1), amount);
    value_name = de_backslah_n(value_name);


    temptemp = root;

    while(temptemp != NULL){
        
        if( (strcmp(temptemp->object_data, object_name) == 0) 
            && (strcmp(temptemp->property_data, property_name) == 0) ){
                
                temptemp->value_data = (char*) realloc(temptemp->value_data, (sizeof(char)) * (strlen(value_name) +1) );
                strcpy(temptemp->value_data, value_name);
        }

        temptemp = temptemp->next;    
    }

    //result(root);

    // free(object_name);
    // free(property_name);
    // free(value_name);
}


char* de_backslah_n(char* cutphrase){

    int i = 0;
    int j = 0;
    output = cutphrase;


    while(i < strlen(cutphrase)){
        if (cutphrase[i]!='\n')                           
            output[j]=cutphrase[i];                     
        else
            j--;

        i++;
        j++;
    }                           
    
    output[j] = 0;

    return output;
}



void addNode(ListNode *temp){
    temp->next = NULL;

    if (root == NULL){
        root = (ListNode*)malloc(sizeof(ListNode));
        
        root->object_data = (char*) malloc( (sizeof(char)) * strlen(temp->object_data) );
        root->property_data = (char*) malloc( (sizeof(char)) * strlen(temp->property_data) );
        root->value_data = (char*) malloc( (sizeof(char)) * strlen(temp->value_data) );

        strcpy(root->object_data, temp->object_data);
        strcpy(root->property_data, temp->property_data);
        strcpy(root->value_data, temp->value_data);
    }
    else{
        ListNode *temptemp = root;
        
        //Create struct
        ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));

        while(temptemp->next != NULL)
            temptemp = temptemp->next;
        
        //Malloc
        newNode->object_data = (char*) malloc( (sizeof(char)) * strlen(temp->object_data) );
        newNode->property_data = (char*) malloc( (sizeof(char)) * strlen(temp->property_data) );
        newNode->value_data = (char*) malloc( (sizeof(char)) * strlen(temp->value_data) );
        
        //Copy the data
        strcpy(newNode->object_data, temp->object_data);
        strcpy(newNode->property_data, temp->property_data);
        strcpy(newNode->value_data, temp->value_data);

        //Set the node
        temptemp->next = newNode;
    }
}
