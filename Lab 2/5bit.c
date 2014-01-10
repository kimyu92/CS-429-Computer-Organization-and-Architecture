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
int encode(FILE *thisfile);

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


int encode(FILE *thisfile){
	int aByte;
	int howManyBytes;
	int howManytoShift;

	int move = 0;
	int startIndex = 0;

	unsigned int checking;
	unsigned char mask;
	unsigned char temp = 0;
	static unsigned char bufferingBit[40];

	int i = 0; 
	int k = 0;
	int l = 0;

	output = fopen("result.txt","w+");

	while ( (aByte = fgetc(thisfile)) != EOF ){
		
		//Placing 8bits in the char array
		while( i < 8){
			howManytoShift = 7 - i;
			mask = aByte >> howManytoShift;
			bufferingBit[ startIndex + i] = (mask & 0x01);
			i++;
		}
		
		//reset the bit index
		i = 0;

		if (startIndex < 40)
			startIndex = startIndex + 8;


		//When my array is full tank
		if((startIndex) == 40){

			//Read five bit until 5 bytes reached
			for (k = 0; k < 40; k++){
    			move = 4 - (k % 5);

    			if(k == 5)
    				temp = 0;
    			
    			temp |= ((bufferingBit[k] & 0x1) << move);


				if( (k + 1) % 5 == 0 ){
					if (temp < 26)
						temp = temp + 'A';
					else
						temp = temp - 26;

					printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

           			temp = temp & 0;          			
           		}
			}

			k = 0;
		}

		//Resetting the array
		if (startIndex == 40){
		 	startIndex = 0;

		 	for(l = 0; l < 40 ; l++)
		 		bufferingBit[l] = 0;
		}	
	}

	//Reuse the index of i
	i = 0;

	//printf("This is so cool %d\n", startIndex);
	//Padding the last remaining bytes
	while(i < startIndex){
			move = 4 - (i % 5);
			
			if(i == 5)
				temp = 0;

			//Padding a bit from array
			temp |= ((bufferingBit[i] & 0x1) << move);

			if( (i + 1) % 5 == 0){
				if (temp < 26)
					temp = temp + 'A';
				else
					temp = temp - 26;

				printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

				temp = temp & 0;
			}
			else if( (i + 1) % startIndex == 0){

				if (temp < 26)
					temp = temp + 'A';
				else
					temp = temp - 26;

				printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

				temp = temp & 0;
			}


		i++;
	}






	printf("End of the encoding\n");
	return 0;
}