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
Boolean decodingInstruction = FALSE;

void doProcess(String filename);
void encode(FILE *thisfile);
void decode(FILE *thisfile);
int printTheNextLine(int line);
//================================
//End of Declaration
//================================


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
    

    if (!Open_Input_File(filename))
    	return;
    else{
    	//printf("File can be open\n");

    	input = fopen(filename, "r");
    	

    	if(decodingInstruction)
    		decode(input);
    	else
    		encode(input);
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

    	if (**argv == '-'){
    		//scanargs(*argv);
    		decodingInstruction = TRUE;
    	}
    	else{
            
            filenamenotgiven = FALSE;
            input = fopen(*argv,"r");

            if (input == NULL){
            	fprintf (stderr, "\nCan't open %s\n",*argv);
            	printf ("Please retry again.\n\n");
            }
            
            doProcess(*argv);
            fclose(input);
            
        }

        
    }

    if(decodingInstruction)
    	decode(input);
    else
    	encode(input);

    exit(0);
}

//Encoding
void encode(FILE *thisfile){
	int aByte;
	int howManyBytes;
	int howManytoShift;

	int move = 0;
	int startIndex = 0;

	unsigned int checking;
	unsigned char mask;
	int temp;
	static unsigned int bufferingBit[40];

	int i = 0; 
	int k = 0;
	int l = 0;
	int line = 0;

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
					line++;

					if (temp < 26){
						temp = temp + 'A';
						printf("%c", temp);
					}
					else{
						temp = temp - 26;
						printf("%d", temp);
					}

					line = printTheNextLine(line);

					//printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

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
				line++;

				if (temp < 26){
					temp = temp + 'A';
					printf("%c", temp);
				}
				else{
					temp = temp - 26;
					printf("%d", temp);
				}

				line = printTheNextLine(line);
				temp = temp & 0;
			}
			else if( (i + 1) % startIndex == 0){
				line++;

				if (temp < 26){
					temp = temp + 'A';
					printf("%c", temp);
				}
				else{
					temp = temp - 26;
					printf("%d", temp);
				}

				//printf(" What is that shit Worrkkkss  %c  %d\n", temp, temp);

				line = printTheNextLine(line);
				temp = temp & 0;
			}

		i++;	
	}
	
}


//Decoding
void decode(FILE *thisfile){
	int aByte;
	int howManyBytes;
	int howManytoShift;

	int move = 0;
	int startIndex = 0;

	unsigned int checking;
	unsigned char aftershift;
	int temp;
	static unsigned int bufferingBit[40];

	int i = 0; 
	int j = 0;
	int k = 0;
	int l = 0;
	int line = 0;

	//printf("this is so cool decoding method\n everything stat here \n\n");
	//output = fopen("result.txt","w+");

	while ( (aByte = getc(thisfile)) != EOF ){
		
		//printf("I am reading this shit   %d %c\n", aByte, aByte);
		
		if (aByte != 10){

			//Retrieve a byte depends on the encoded code 
			if (aByte >= 65)
				aByte = aByte - 65;
			else
				aByte = aByte - 22;


			//Placing 5bits in the char array
			while( i < 5){

				howManytoShift = 4 - i;
				aftershift = aByte >> howManytoShift;

				bufferingBit[ startIndex + i] = (aftershift & 1);

				i++;
			}

			
			// //Debug
			// for (j = 0; j < 40; j++){

			// 	printf("%d", bufferingBit[j]);

			// 	if( ((j+1) % 5) == 0 && j!=0 )
			// 		printf("  ");

			// }
			// printf("\n");



			//reset the bit index
			i = 0;

			if (startIndex < 40)
				startIndex = startIndex + 5;

			printf("Inserted index:       %d\n\n", startIndex);


			//When my array is full tank
			if((startIndex) == 40){

				//printf("What shit is coming out\n");

				//Read five bit until 5 bytes reached
				for (k = 0; k < 40; k++){
    			
					move = 7 - (k % 8);

					if(k == 8)
						temp = 0;

					temp |= ((bufferingBit[k] & 0x1) << move);


					if( (k + 1) % 8 == 0 ){
						line++;
						printf("%c", temp);
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
	}

	//Reuse the index of i
	i = 0;

	printf("This is so cool %d\n", startIndex);
	
	//Padding the last remaining bytes
	while(i < startIndex){
			move = 7 - (k % 8);

			if(i == 8)
				temp = 0;

			//Padding a bit from array
			temp |= ((bufferingBit[i] & 0x1) << move);

			if( (i + 1) % 8 == 0){
				line++;
				printf("%c", temp);
				temp = temp & 0;
			}
		i++;	
	}


}


//Print the next line when it is more than 71 chars
int printTheNextLine(int line){
	if( line > 71 ){
		printf("\n");
		return 0;
	}

	return line;
}
