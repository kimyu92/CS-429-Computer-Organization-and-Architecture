#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
	int howManyBytes;
	unsigned int checking;
	int howManytoShift;

	int i = 0;
	int j;
	int k;
	int move;
	int startIndex = 0;
	unsigned char mask;

	

	unsigned char temp;
	static unsigned char bufferingBit[40];

	// char  abccc = 'F';
	// char  abccc1 = 'f';
	// char  abccc2 = 'f';

	output = fopen("result.txt","w+");
	//printf("%x\n", mask);

	// printf("%d\n", (abccc >> 7)&1 );
	// printf("%d\n", abccc);
	// printf("%d\n", (abccc >> 6)&1 );
	// printf("%d\n", abccc);
	// printf("%d\n", (abccc >> 5)&1);
	// printf("%d\n", ((abccc <<3 )>> 7)&1);
	// printf("%d\n", ((abccc <<4 )>> 7)&1);
	// printf("%d\n", ((abccc <<5 )>> 7)&1);
	// printf("%d\n", ((abccc <<6 )>> 7)&1);
	// printf("%d\n\n", ((abccc <<7 )>> 7)&1);


	while ( (aByte = fgetc(thisfile)) != EOF ){
		
		fprintf(stdout,"%x\n\n", aByte);

		while( i < 8){
			howManytoShift = 7 - i;
			mask = aByte >> howManytoShift;
			
			printf("%d\n", (mask&1));

			for (j = 0; j < 40; j++){
				printf("%x", bufferingBit[i]);

				if(j ==19)
					printf("\n");
			}
			printf("\n");

			bufferingBit[ startIndex + i] = (mask & 0x01);
			i++;
		}
		

		i = 0;
		
		//printstuff(bufferingBit);

		// for (k = 0; k < 39; k++)
		// 	printf("%c", bufferingBit[k]);

		// printf("\n");
		//bufferingBit |= aByte << howManyBytes;

		if(startIndex == 40){
			printf("PAAAAAAAAAAAAAAAAAAAAAAAAAASDSS\n");
			for (k = 0; k < 40; k++){
    			
    			move = i % 8;

    			temp |= (bufferingBit[k] & 1) << move;
				//temp |= (bufferingBit[k] == '1') << move;
				

				if( (k + 1)%8 == 0){	
					checking = fwrite (&temp, 1, 1, output);
           			printf("%d\n\n", checking);
           			temp = 0;
           		}

			}

			k = 0;
			startIndex = 0;
		}


		startIndex = startIndex + 8;

		
	}

	printf("End of the encoding\n");
}

void printstuff(unsigned char bufferingBit[]){
	int i;

	for (i = 0; i < 40; i++){
			printf("%x", bufferingBit[i]);
	}

	printf("\n");

}

// //Get the 5 bit
		// bufferingBit = (aByte >> 3) & ( ~(1 << 7 >> 2) ); 


		// //Dont forget the 3 bit
		// //Carry to next one
		// remainingBit = aByte << 5;


		// //Add 65
		// bufferingBit = bufferingBit + 65;



		// //Check out of bound or not
		// //If in-bound, add 65
		// //If not, subtract 26

		// if(bufferingBit > 90)
		// 	bufferingBit = (bufferingBit - 65) - 26;



		//Make into 8 bit
		//Write into the file
		// printf("CHECKING     \n");
		// checking = fwrite (&bufferingBit, 1, 1, output);
		
		// printf("%d\n\n", checking);

		// //fwrite (&bit_buffer, 1, 1, f);
		// bufferingBit = 0;



            	// temp = bufferingBit;

            	// temp = ((bufferingBit >> 31) & mask) & 0xFF; 
            	// checking = fwrite (&temp, 1, 1, output);
            	// printf("%d\n\n", checking);

            	// temp = ((bufferingBit >> 23) & mask) & 0xFF;; 
            	// checking = fwrite (&temp, 1, 1, output);
            	// printf("%d\n\n", checking);

            	// temp = ((bufferingBit >> 15) & mask) & 0xFF;; 
            	// checking = fwrite (&temp, 1, 1, output);
            	// printf("%d\n\n", checking);

            	// temp = ((bufferingBit >> 7) & mask) & 0xFF;; 
            	// checking = fwrite (&temp, 1, 1, output);
            	// printf("%d\n\n", checking);

            	// temp = ((bufferingBit >> 0) & mask) & 0xFF;;
            	// checking = fwrite (&temp, 1, 1, output);
            	// printf("%d\n\n", checking);