//Name   : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA     : Ji Hong

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *input1;
FILE *input2;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

#define FACT F
#define QUEST Q

typedef char *STRING;

typedef struct questInfo{
    STRING object_data;
    STRING property_data;
    STRING value_data;
    
    struct questInfo *nextNode;
}ListNode;

static int aByte = (sizeof(char));

//Root Node for question file
//Root Node for fact file
ListNode *root = NULL;
ListNode *root2 = NULL;

////Functions scopes
static int decision(FILE *in1, FILE *in2);

//Parsing
//Parsing and Storing linked list
void parseFactFile();
void parseQuestFile(STRING question, int posQ, int posCol);

//Parsing
//Parsing and Storing linked list
void getFromQFile();
void getFacts(STRING line, int posF,int posCol,int posEqual);

//Update the value by checking linked list
//Print out the result
void updateValue();

//Free the linkedlist
void freeLinkedList(ListNode *passedNode);

//==================================
//End of Declaration
//==================================



//Main Method
int main(int argc, char **argv){
 
   while(argc > 1){
    argc --;
    argv ++;
    
    input1 = fopen(*argv,"r");
    argc--;
    argv++;
    
    input2 = fopen(*argv,"r");

    switch( decision(input1, input2) ){
        case 0:
            fprintf(stderr, "Both files %s , %s couldnt be read. Sorry. \n",argv[1],argv[2] );
            return 0;
            break;
        case 1:
            getFromQFile();
            updateValue();

            freeLinkedList(root);
            freeLinkedList(root2);

            fclose(input2);
            return 0;
            break;
        case 2:
            getFromQFile();
      
            parseFactFile();
            updateValue();
      
            freeLinkedList(root);
            freeLinkedList(root2);
      
            fclose(input1);
            fclose(input2);
            return 0;
            break;
        default:
            input2 = stdin;
            getFromQFile();
      
            parseFactFile();
            updateValue();
      
            freeLinkedList(root);
            freeLinkedList(root2);
       
            fclose(input1);
            return 0;
            break;
    }
  }
  
  return 0;
}


static int decision(FILE* in1, FILE* in2){

    if(in1 == NULL && in2 == NULL)
        return 0;
    else if(in1==NULL && in2 != NULL)
        return 1;
    else if(in1 != NULL && in2 != NULL)
        return 2;
    else
        return 3;
} 


//Free the linkedlist
void freeLinkedList(ListNode *passedNode){
 
  ListNode *temp;
 
  while(passedNode){
    //Swaping
    //Grab the current node
    //Move to next node
    temp = passedNode;
    passedNode = temp->nextNode;
 
    free(temp->object_data);
    free(temp->property_data);
    free(temp->value_data);
    
    free(temp);
  }

}



void getFromQFile(){
    
    Boolean checkEndLine = FALSE, endFile = FALSE;
    
    int findQ = -1, findColon = -1, count = 0, i;
    
    STRING arrString;
    char letter;

    while(!endFile){
        letter = fgetc(input2);

        if(letter == EOF)
            endFile = TRUE;

        if(!endFile){

            if(letter!= '\n')
                count++;

            if(count == 1 && letter == 'Q' && findQ == -1){
                findQ = count-1;
                arrString = (STRING)malloc( aByte );
            }

            if(letter == '\n')
                checkEndLine = TRUE;

            if(letter == ':' && findColon < 0)
                findColon = count-1;

            if(findQ != -1 && letter != '\n'){

                if(count > strlen(arrString) )
                    arrString = (STRING)realloc(arrString, (count + 1) * aByte );

                arrString[count-1] = letter;
            }
        }

        if(checkEndLine || endFile){

            if(findQ != -1 && findColon != -1){

                arrString = (STRING)realloc(arrString, (count + 1) * aByte );
                arrString[count]='\0';

                parseQuestFile(arrString,findQ,findColon);
            }

            if(findQ != -1)
                free(arrString);

            if(checkEndLine){
                findQ = -1;
                findColon = -1;
                checkEndLine = FALSE;
                count = 0;
            }
        }
    }
}


//Process the question file
//Parse the question file
//Store inside the ListNode and chained up as a linkedlist
void parseQuestFile(STRING question, int posQ, int posCol){
    Boolean check = FALSE;

    int i =0;
    int afterQ = posQ + 1;

    int beforeCol = posCol - 1;
    int afterCol = posCol + 1;

    int endBack = strlen(question) - 1;

    //Position
    int objBeg = -1, objEnd = -1, propBeg = -1, propEnd = -1;

    STRING unknown = (STRING) "unknown";

    while(!check){

        if(objBeg < 0 && question[afterQ] != ' ' )
            objBeg = afterQ;

        if(objEnd < 0 && question[beforeCol] != ' ' ) 
            objEnd = beforeCol;
        if(propBeg < 0 && question[afterCol] != ' ' )
            propBeg = afterCol;
        if(propEnd < 0 && question[endBack] != ' ' )
            propEnd = endBack;

        if(objBeg > 0 && objEnd > 0 && propBeg > 0 && propEnd > 0)
            check = TRUE;

        afterQ++;
        beforeCol--;
        afterCol++;
        endBack--;
    }

    //Adding the node
    //The head is null
    //Otherwise
    if(root == NULL){

        root = (ListNode*) malloc(sizeof(ListNode) * aByte);
        root->object_data = (STRING) malloc( (objEnd - objBeg + 1) * aByte );
        root->property_data = (STRING) malloc( (propEnd - propBeg + 1) * aByte );
        root->value_data = (STRING) malloc( strlen(unknown) * aByte );
    
        //Set the next node to null to prevent infinite loop
        root->nextNode = NULL;

        strncat(root->object_data, question+objBeg, (objEnd - objBeg + 1));
        strncat(root->property_data, question+propBeg, (propEnd - propBeg + 1));
        strncat(root->value_data, unknown, strlen(unknown));
    }
    else{
        ListNode *tempNode = root;

        while(tempNode->nextNode != NULL)
            tempNode = tempNode->nextNode;

        ListNode *newNode = (ListNode*)malloc( sizeof(ListNode) * aByte);
    
        newNode->object_data = (STRING) malloc( (objEnd - objBeg + 1) * aByte );
        newNode->property_data = (STRING) malloc( (propEnd - propBeg +1) * aByte );
        newNode->value_data = (STRING) malloc( strlen(unknown) * aByte);
    
        //Set the next node to null to prevent infinite loop
        newNode->nextNode = NULL;

        strncat(newNode->object_data,question+objBeg,(objEnd - objBeg + 1));   
        strncat(newNode->property_data,question+propBeg,(propEnd - propBeg + 1));  
        strncat(newNode->value_data,unknown,strlen(unknown));
    
        tempNode->nextNode = newNode;
    }
}



void parseFactFile(){

    Boolean checkEndLine = FALSE, endFile = FALSE;

    //Position
    int findF = -1, findColon = -1, findEqual = -1;
    int count = 0, i;

    char letter;
    STRING arrString;

    while(!endFile){
        letter = fgetc(input1);

        if(letter == EOF)
            endFile = TRUE;

        if(!endFile){

            if(letter!= '\n')
                count++;

        if(count == 1 && letter == 'F' && findF == -1){
            findF = count - 1;
            arrString = (STRING)malloc( aByte );
        }

        if(letter == '\n')
            checkEndLine = TRUE;

        if(letter == ':' && findColon < 0)
            findColon = count-1;

        if(letter == '=' && findEqual < 0)
            findEqual = count-1;

        if(findF != -1 && letter != '\n'){

                if(count > strlen(arrString) )
                    arrString = (char*)realloc(arrString, (count + 1) * aByte);

                arrString[count-1] = letter;
            }
        }

        if(checkEndLine || endFile){

            if(findF != -1 && findColon != -1 && findEqual != -1){
                arrString = (char*)realloc(arrString, (count + 1) * aByte);
                arrString[count]='\0';
                getFacts(arrString,findF,findColon,findEqual);
            }

            if(findF != -1)
                free(arrString);

            if(checkEndLine){
                findF = -1, findColon = -1, findEqual = -1;
                checkEndLine = FALSE;
                count = 0;
            }
        }
    }
}



void getFacts(STRING line, int posF, int posCol, int posEqual){

    int afterCrit = posF + 1;
    
    int beforeCol = posCol - 1;
    int afterCol = posCol + 1;
    
    int beforeEqual = posEqual - 1;
    int afterEqual = posEqual + 1;
    
    int endBack = strlen(line) - 1;
    int i =0;

    int objBeg = -1, objEnd = -1, propBeg = -1, propEnd = -1, valBeg = -1, valEnd = -1;

    Boolean done = FALSE, check = FALSE;

    while(!check){

        if(objBeg < 0 && line[afterCrit] != ' ' )
            objBeg = afterCrit;
    
        if(objEnd < 0 && line[beforeCol] != ' ' ) 
            objEnd = beforeCol;

        if(propBeg < 0 && line[afterCol] != ' ' )
            propBeg = afterCol;

        if(propEnd < 0 && line[beforeEqual] != ' ' )
            propEnd = beforeEqual;

        if(valBeg < 0 && line[afterEqual] != ' ')
            valBeg = afterEqual;

        if(valEnd < 0 && line[endBack] != ' ')
            valEnd = endBack;

        if(objBeg > 0 && objEnd > 0 && propBeg > 0 && propEnd > 0 && valBeg >0 && valEnd >0)
            check = TRUE;

        afterCrit++;
        beforeCol--;

        afterCol++;
        beforeEqual--;

        afterEqual++;
        endBack --;
    }


    //Adding the node
    //The head is null
    //Otherwise
    if(root2 == NULL){

        root2 = (ListNode*)malloc(sizeof(ListNode)* aByte );
        root2->object_data = (STRING)malloc( (objEnd - objBeg + 1) * aByte );
        root2->property_data = (STRING)malloc( (propEnd - propBeg + 1) *aByte);
        root2->value_data = (STRING)malloc( (valEnd - valBeg + 1) * aByte );

        //Set the next node to null to prevent infinite loop
        root2->nextNode = NULL;

        strncat(root2->object_data, line + objBeg, (objEnd - objBeg + 1));
        strncat(root2->property_data, line + propBeg, (propEnd - propBeg + 1));
        strncat(root2->value_data, line + valBeg, (valEnd - valBeg + 1));
    }

    else{
        ListNode *tempNode = root2;

        while(tempNode->nextNode != NULL)
            tempNode = tempNode->nextNode;

        ListNode *newNode = (ListNode*) malloc(sizeof(ListNode) * aByte);
        newNode->object_data = (STRING) malloc( (objEnd - objBeg + 1) * aByte);
        newNode->property_data = (STRING) malloc( (propEnd - propBeg + 1) * aByte);
        newNode->value_data = (STRING) malloc( (valEnd - valBeg + 1) * aByte);

        //Set the next node to null to prevent infinite loop
        newNode->nextNode = NULL;

        strncat(newNode->object_data, line + objBeg, (objEnd - objBeg + 1));
        strncat(newNode->property_data, line + propBeg, (propEnd - propBeg + 1));
        strncat(newNode->value_data, line + valBeg, (valEnd - valBeg + 1));

        tempNode->nextNode = newNode;
    }
}



void updateValue(){

    ListNode *temp = root, *temp1 = root;

    //Check and update the value on the question struct
    while(temp != NULL){
        ListNode *temp2 = root2;

        while(temp2 != NULL){

            if( (strcmp(temp2->object_data,temp->object_data) == 0) 
                && (strcmp(temp2->property_data,temp->property_data) == 0) ){

                free(temp->value_data);
                temp->value_data = (STRING)malloc( (strlen(temp2->value_data)) * aByte);
                strcpy(temp->value_data,temp2->value_data);
            }

            temp2 = temp2 ->nextNode;
        }

        temp = temp ->nextNode;
    }

    //Print out the final result
    while(temp1 != NULL){

        printf("F %s:", temp1->object_data);
        printf(" %s", temp1->property_data);
        printf("=%s\n", temp1->value_data);

        temp1 = temp1 ->nextNode;
    }

}







