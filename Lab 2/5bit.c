#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *input;
FILE *output;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

typedef char *String;

Boolean d_option = FALSE; /* */
Boolean e_option = FALSE; /* */

void doProcess(String filename);
void encode(FILE *thisfile);

//================================
//End of Declaration
//================================


/*Scan command argument*/
void scanargs(char *s){

	//check each character of the option list for its meaning.
	while (*s != '\0')
		switch (*s++){
            case '-': /* option indicator */
				break;
            case 'd': /* print all; even controls */
				d_option = TRUE;
				break;
            case 'e': /* print mismatched pairs: {} and () */
				e_option = TRUE;
				break;
			default:
				fprintf(stderr, "\nBad option %c from user\n", s[-1]);
				fprintf(stderr, "       -d -- decode\n");
				fprintf(stderr, "       -e -- encode into 5 bit\n\n");
				exit(1);
		}
}


/*Check the file input*/
Boolean Open_Input_File(char *name){
	if (name == NULL)
		input = stdin;
	else{
		input = fopen(name, "r");

		if (input == NULL){
			fprintf(stderr, "Can't open %s\n", name);
			return(FALSE);
		}
	}
	return(TRUE);
}



/*Validate file can be open or not*/
void doProcess(String filename){
    FILE *thisfile;

    if (!Open_Input_File(filename))
    	return;
    else{
    	printf("File can be open\n");

    	thisfile = fopen(filename, "r");
    	encode(thisfile);

    	fclose(thisfile);
    	//Call the encoding method
    }
}


int main(int   argc, char **argv){
	Boolean filenamenotgiven = TRUE;

	/* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments. 
    */

    while (argc > 1){

    	argc--, 
    	argv++;

    	if (**argv == '-')
    		scanargs(*argv);
    	else{
            
            filenamenotgiven = FALSE;
            input = fopen(*argv,"r");

            if (input == NULL){
            	fprintf (stderr, "\nCan't open %s\n",*argv);
            	printf ("Please retry again.\n\n");
            }
            else{
            	doProcess(*argv);
                fclose(input);
            }
        }

        if (filenamenotgiven){
        	input = stdin;
        	doProcess(NULL);
    	}

    	exit(0);
	}


}


void encode(FILE *thisfile){
	int aByte;
	int checking;

	unsigned char bufferingBit;
	unsigned char remainingBit;

	Boolean is8Bit = FALSE;

	//output = fopen("result.txt","w+");
	output = fopen("result.txt","w+");

	while ( (aByte = fgetc(thisfile)) != EOF ){
		

		fprintf(stdout,"%x\n", aByte);

		//Get the 5 bit
		bufferingBit = (aByte >> 3) & ( ~(1 << 7 >> 2) ); 


		//Dont forget the 3 bit
		//Carry to next one
		remainingBit = aByte << 5;


		//Add 65
		bufferingBit = bufferingBit + 65;



		//Check out of bound or not
		//If in-bound, add 65
		//If not, subtract 26

		if(bufferingBit > 90)
			bufferingBit = (bufferingBit - 65) - 26;



		//Make into 8 bit
		//Write into the file
		printf("CHECKING     \n");
		checking = fwrite (&bufferingBit, 1, 1, output);
		
		printf("%d\n\n", checking);







		//fwrite (&bit_buffer, 1, 1, f);
		bufferingBit = 0;

	}

	printf("End of the encoding\n");
}