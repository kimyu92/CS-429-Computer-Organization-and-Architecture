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

	int i, j, k, l;
	int move;
	int startIndex = 0;
	unsigned char mask;
	unsigned char temp = 0;
	static unsigned char bufferingBit[40];


	output = fopen("result.txt","w");


	while ( (aByte = fgetc(thisfile)) != EOF ){
		
		//fprintf(stdout,"I am reading this byte %d which is %c\n\n", aByte, aByte);

		//Placing 8bits in the char array
		while( i < 8){
			howManytoShift = 7 - i;
			mask = aByte >> howManytoShift;
			bufferingBit[ startIndex + i] = (mask & 0x01);
			i++;
		}
		
		//reset the bit index
		i = 0;
		
		//printstuff(bufferingBit);
		//for (j = 0; j < 39; j++)
		//  printf("%d", bufferingBit[j]);

		printf("Start Index: %d\n", startIndex);

		if (startIndex < 40)
			startIndex = startIndex + 8;


		//When my array is full tank
		if((startIndex) == 40){
			printf("This is array 40 time to add\n");

			for (j = 0; j < 40; j++){
				printf("%x", bufferingBit[j]);

				if(j ==19)
					printf("\n");
			}
			printf("\n");
			

			//Read five bit until 5 bytes reached
			for (k = 0; k < 40; k++){
    			
    			move = 4 - (k % 5);

    			if(k == 5){
    				temp = 0;
    				printf("Resetting   %d\n", temp);

    			}

    			temp |= ((bufferingBit[k] & 0x1) << move);

				
				printf(" Worrkkkss  %d\n", temp);

				if( (k + 1) % 5 == 0 || ( ((k + 1) % startIndex) == 0) ){
					printf(" Worrkkkss 2 %d\n", temp);
					
					if (temp < 26)
						temp = temp + 'A';
					else
						temp = temp - 26;


					printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

					//checking = fwrite (temp, 1, 1, output);
           			//printf("What did you write %d\n\n", temp);
           			//printf(")
           			temp = temp & 0;          			
           		}
           		

			}

			k = 0;
			
		}

		
		
		if (startIndex == 40){
		 	startIndex = 0;

		 	for(l = 0; l < 40 ; l++)
		 		bufferingBit[l] = 0;
		}

		
	}


	//debugging outside array
	for (j = 0; j < 40; j++){
		printf("%x", bufferingBit[j]);

		if(j ==19)
			printf("\n");
	}
	printf("\n");

	i = 0;

	printf("This is so cool %d\n", startIndex);





	//padding
	while(i < startIndex){

		printf("Enter here like a boss\n");
			move = 4 - (i % 5);
			
			if(i == 5){
				temp = 0;
				printf("Resetting   %d\n", temp);
			}

			temp |= ((bufferingBit[i] & 0x1) << move);


			printf(" Worrkkkss  %d\n", temp);

			if( (i + 1) % 5 == 0){
				printf(" Worrkkkss 2 %d\n", temp);

				if (temp < 26)
					temp = temp + 'A';
				else
					temp = temp - 26;


				printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

					//checking = fwrite (temp, 1, 1, output);
           			//printf("What did you write %d\n\n", temp);
           			//printf(")
				temp = temp & 0;
			}
			else if( (i + 1) % startIndex == 0){
				printf(" Worrkkkss 2 %d\n", temp);

				if (temp < 26)
					temp = temp + 'A';
				else
					temp = temp - 26;


				printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

					//checking = fwrite (temp, 1, 1, output);
           			//printf("What did you write %d\n\n", temp);
           			//printf(")
				temp = temp & 0;
			}



		
		i++;
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