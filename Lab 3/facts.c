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


char *res;


//Functions scope
void process();

//Grab String
char *somehow_process_q_line();
char *somehow_get_a_line();
char* deblank(char* input);
char* de_backslah_n(char* cutphrase);

//Get fact and store them
void get_the_question(String thisString);
void get_a_fact(String thisString);
char *substr(const char *data_str, int pos_start, int pos_end);

//Search the linkedlist and update
void search_in_linkedlist();

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
ListNode *temp;

//For the get_the_question or get_a_fact methods
char *object_name, *property_name, *value_name;

//get_a_fact
char *s;

//For the deblank
char *output;

//addNode
ListNode *newNode;


//freeMemNodes
ListNode * clearTemp;


//Method prototype from strct
void display(ListNode *r);
void addNode(ListNode *temp);

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


//Main Process
void process(){
    //Get a line
    somehow_process_q_line();
    somehow_get_a_line();
    result(root);
    //freeMemNodes();

    //free(newNode);
    //free(temp);
    //freeList(temp);

    //free(arrStr);

    //free(res);
}

void freeMemNodes(ListNode * ptr){

    if(root == NULL)
        return;

    clearTemp = ptr;

    while(clearTemp != NULL){

        ListNode *temp = ptr->next;
        
        free(clearTemp);

        clearTemp = temp;
    }
}

void freeList(ListNode *head){
   ListNode *tmp;

   while (head != NULL){
       tmp = head;
       head = head->next;
       free(tmp);
    }
}

char *somehow_process_q_line(){
    int aByte;
 
    Boolean theStartIsQ = FALSE;
    Boolean countedTheLine = TRUE;
     
    int charInLine = 0;
    int maxCharInLine = 0;

    //Buggy while loop
    //Super buggySSSSSSS

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

    //printf("%d \n",maxCharInLine);
    //printf("%d \n",charInLine);

    //Go back to the head
    rewind(input2);
    //printf("This is whatever size %d\n", maxCharInLine);
    //printf("============================\n\n");
 
    //Allocate memory for fgets
    char *arrStr = (char*) malloc( (sizeof(char)) * (maxCharInLine + 1) );
     
    //Go again my size
    while( (fgets(arrStr, (maxCharInLine + 1), input2)) != NULL ){

        //Skip a blank line
        if(arrStr[0] != '\n' ){

            //printf("This is before     : %s\n", arrStr);                           //Debugging

            //Cutting down the string
            arrStr = deblank(arrStr);
        
            //printf("What is the truncated: %s\n", arrStr);                         //Debugging

            //Getting the fact
            get_the_question(arrStr);
            
        }

    }
    free(arrStr);
}



void get_the_question(String thisString){
    s = thisString;
    int len = strlen(s);

    int pos_colon = -1;
    
    int i;
    int j;

    


    if (s[0] != 'Q')
        return;

    for (i = 0; i < len; i++)
        if (s[i] == ':'){
            pos_colon = i;
            break;
        }
    

    if (pos_colon == -1)
        return;


    //printf("Result of question: \n");                               //Debugging starts
    
    //ListNode *temp = (ListNode*)malloc(sizeof(ListNode));

    temp = (ListNode*)malloc(sizeof(ListNode));

    //Grabbing each element
    object_name = substr(s, 1, pos_colon);
    object_name = de_backslah_n(object_name);
    //printf("%lu\n", strlen(object_name));    
    temp->object_data = object_name;
    //printf("%s\n", object_name);


    property_name = substr(s, pos_colon + 1, len);
    property_name = de_backslah_n(property_name);
    //printf("%lu\n", strlen(property_name));
    temp->property_data = property_name;
    //printf("%s\n", property_name);


    //value_name = (char *) malloc( (sizeof(char)) * 8 );
    value_name = (String) "unknown";
    //printf("%s\n", property_name);

    temp->value_data = value_name;
    //printf("%s\n\n", value_name);


    addNode(temp);
    freeList(temp);
    //freeMemNodes(newNode);
    //free(res);
    //free(temp);

}


//Getting a string
char *somehow_get_a_line(){
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
    char *arrStr = (char*) malloc( (sizeof(char)) * (maxCharInLine + 1) );
     
    //Go again my size
    while ( (fgets(arrStr, (maxCharInLine + 1), input)) != NULL ){

        //Skip a blank line
        if(arrStr[0] != '\n' ){

            //printf("This is before     : %s\n", arrStr);                           //Debugging

            //Cutting down the string
            arrStr = deblank(arrStr);
        
            //printf("What is the truncated: %s\n", arrStr);                         //Debugging

            //Getting the fact
            get_a_fact(arrStr);
        }

    }

    free(arrStr);
}



//Cutdown the string
char* deblank(char* input){
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



void get_a_fact(String thisString){
    s = thisString;
    int len = strlen(s);

    int pos_colon = -1;
    int pos_equ = -1;
    
    int i;
    int j;


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
    object_name = substr(s, 1, pos_colon);
    object_name = de_backslah_n(object_name);


    property_name = substr(s, pos_colon + 1, pos_equ);
    property_name = de_backslah_n(property_name);



    value_name = substr(s, pos_equ + 1, len);
    value_name = de_backslah_n(value_name);


    update_value(object_name, property_name, value_name);

    //free(res);
    //free(temp);
}


char* de_backslah_n(char* cutphrase){
    //char *output = cutphrase;
    //char *output = (char*) malloc( (sizeof(char)) * strlen(cutphrase) );
    //output = cutphrase;
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
     
    //ListNode *newNode = (ListNode*) malloc(sizeof(ListNode));
    newNode = (ListNode*) malloc(sizeof(ListNode));
 
    newNode->object_data = temp->object_data;
    newNode->property_data = temp->property_data;
    newNode->value_data = temp->value_data;
    temp->next = NULL;
     

    if (root == NULL)
        root = newNode;
    else{
        ListNode *temptemp = root;
         
        while(temptemp->next != NULL)
            temptemp = temptemp->next;
         
        temptemp->next = newNode;
    }
 
    display(root);
}



//Print of linkedlist
void display(ListNode *r){
    int i  = 0; 
    //r = root;
    if(r == NULL)
        return;
    
    while(r != NULL){
        // printf("\n Node %d: ", i);
        // printf("%s ",r->object_data);
        // printf("%s ",r->property_data);
        // printf("%s ",r->value_data);
        r=r->next;

        i++;
    }

    //printf("\n");
}


void result(ListNode *r){
    //r = root;
    if(r == NULL)
        return;
    
    while(r != NULL){
        printf("F %s:",r->object_data);
        printf(" %s=",r->property_data);
        printf("%s\n",r->value_data);
        r=r->next;

    }

    //printf("\n");
}

//Update the value
void update_value(String object_name, String property_name, String value_name){

    ListNode *temptemp = root;
    
    //printf("Updating the value.....    \n");
    
    while(temptemp->next != NULL){
        // printf("This object is\n");
        // printf("%s\n", temptemp->object_data);
        // printf("%s\n", temptemp->property_data);
        // printf("%s\n", temptemp->value_data);
        
        if( strcmp(temptemp->object_data, object_name) == 0 ){
            //printf("no1 \n");
            if( strcmp(temptemp->property_data, property_name) == 0 )
                temptemp->value_data = value_name;
        }

        temptemp = temptemp->next;    
    }

    display(root);
    //printf("\n");

}


//Helper for get_a_fact
//Fetch the data / value
char *substr(const char *data_str, int pos_start, int pos_end){

    int amount = pos_end - pos_start;

    res = (char*) malloc( (sizeof(char)) * (amount+1) );
    
    memcpy(res, data_str + pos_start, amount);

    return res;
}
