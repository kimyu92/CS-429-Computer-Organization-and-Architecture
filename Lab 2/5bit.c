//Name 	 : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA   	 : Ji Hong


#include <stdio.h>
#include <stdlib.h>

FILE *input;

typedef short Boolean;
#define TRUE 1
#define FALSE 0

typedef char *String;

Boolean decodingInstruction = FALSE;

void encode();
void decode();
int printTheNextLine(int line);
//================================
//End of Declaration
//================================


int main(int   argc, char **argv){
	Boolean hasFile = FALSE;
	/* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments. 
    */

    while (argc > 1 && !hasFile){

    	argc--, 
    	argv++;

    	if (**argv == '-'){
    		decodingInstruction = TRUE;
    	}
    	else{
            
            input = fopen(*argv,"r");

            if (input == NULL){
            	fprintf (stderr, "\nCan't open %s\n",*argv);
            	printf ("Please retry again.\n\n");
            }
            hasFile = TRUE;
        }

        
    }

    if (!hasFile)
    	input = stdin;

    if(decodingInstruction){
    	decode();
    	fclose(input);
    }
    else{
    	encode();
    	fclose(input);
    }


    exit(0);
}

//Encoding
void encode(){
	int aByte;
	int howManyBytes;
	int howManytoShift;

	int move = 0;
	int startIndex = 0;
	int countFourtyBit = 0;

	int checking;
	int mask;
	int temp;
	static unsigned int bufferingBit[40];

	int i = 0; 
	int k = 0;
	int l = 0;
	int line = 0;

	while ( (aByte = fgetc(input)) != EOF ){
		
		//Placing 8bits in the char array
		while( i < 8){
			howManytoShift = 7 - i;
			mask = aByte >> howManytoShift;
			bufferingBit[ startIndex + i] = (mask & 0x01);
			
			countFourtyBit++;
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

				line = printTheNextLine(line);
				temp = temp & 0;

				printf("\n");
			}

		i++;	
	}

	//When it is 40 bit and not entering the remaining case
	//print a \n to pass the test cases
	if ( ((countFourtyBit + 1) % 40) == 40){
		printf("\n");
	}

	
}


//Decoding
void decode(){
	int aByte;
	int howManyBytes;
	int howManytoShift;

	int aftershift;
	int temp;
	int bufferingBit[8];

	int readSoMuchOfBit = 0;
	int remaining = 0;
	int writtenChar = 0;
	int j = 0;


	while ( (aByte = fgetc(input)) != EOF ){
		//Retrieve a byte depends on the encoded code
		if (aByte != 10){
			if (aByte >= 65)
				aByte = aByte - 65;
			else
				aByte = aByte - 22;


			readSoMuchOfBit = 0;

			//Placing 5bits in the char array
			while( readSoMuchOfBit < 5){

				//Shifting
				howManytoShift = 4 - readSoMuchOfBit;
				aftershift = aByte >> howManytoShift;
				bufferingBit[ writtenChar ] = (aftershift & 1);


				//Read bit by bit counter
				readSoMuchOfBit++;
				writtenChar++;

				//I have five bit
				if (writtenChar == 8){
					//Resetting the temp
					temp &= 0;

					for (j = 0; j < 8; j++)
						temp |= bufferingBit[j] << (7- j);

					//Write it out
					printf("%c", temp);

					writtenChar = 0;
				}
			}

		}
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