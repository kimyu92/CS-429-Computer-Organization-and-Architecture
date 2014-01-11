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
	int bufferingBit[5];

	int lineBreak = 0;
	int countFortyBits = 0;
	int temp;  		
	
	int maxAmountToShift = 4;

	int arrayIndex = 0; 		
	int howManyBits = 0;
	
	int index; 				
	int shifter;   			
	Boolean newlineCase; 		
	
	int i;
	int j;

	while((aByte = getc(input)) != EOF){

		index = 0;
		howManyBits = 0;
		newlineCase = TRUE;

		//Goes through each bit of aByte and put inside 5bit container
		while(howManyBits < 8){
			countFortyBits++;

			shifter = 7-index;
			i = (aByte >> shifter) & 1;

			bufferingBit[arrayIndex] = i;
			arrayIndex++;
			index++;

			//Go through the array when the array is full tank
			if(arrayIndex == 5){
				temp = 0;

				//Take out the bit
				for(j = 0;j < 5; j++)
					temp |= (bufferingBit[j] << (maxAmountToShift - j));

				//Write it out
				if ( (temp + 65) > 90){
					temp = temp - 26;
					printf("%d", temp);
				}
				else{
					temp = temp + 65;
					printf("%c", temp);
				}

				lineBreak++;
				arrayIndex = 0;
			}

			howManyBits++;
		}

		///Check whether have to go next line or not
		lineBreak = printTheNextLine(lineBreak);
	}

	//The edge case where there are remaining bit
	if (newlineCase == 1)
		if(arrayIndex > 0){
			lineBreak++;
			temp = 0;

			for(i = 0; i < arrayIndex; i++)
				temp |= bufferingBit[i] << (maxAmountToShift - i);
			
			printTheNextLine(lineBreak);

			//Write it out
			if ( (temp + 65) > 90){
				temp = temp - 26;
				printf("%d", temp);
			}
			else{
				temp = temp + 65;
				printf("%c", temp);
			}

			printf("\n");
		}
		//The special case
		else if (lineBreak % 70 != 0)
			printf("\n");

	
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