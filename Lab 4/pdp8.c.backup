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

typedef char *STRING;

static short my_memory[4096];
int program_counter = 0;
int _instruct = 0;
long long cycle_count = 0;

STRING instructions;

int registerA = 0;
int linkBit = 0;
Boolean skipFlag = FALSE;
Boolean IOTFlag = FALSE;

Boolean HALT = FALSE;


//Functions Prototype
Boolean checkFile();
Boolean check_char_within_range(char receivedChar);

//Controller
void process();

//Load Memory
void load_into_memory();
char convert_A_to_F_num(char temp);

//Fetching
void decide_instruction_sets();

int compute_effectiveAddr();
void first_fetch_sets();
void second_fetch_sets();
void third_fetch_sets();


//Printing the result
void printTheResult();


int main(int argc, char **argv){
	if(argc > 3)
		return 0;
	//Non-verbose mode
	else if(argc == 2){
		input1 = fopen(argv[1], "r");

		if(input1 == NULL)
			return 0;

		printf("This obj file\n");
		if (checkFile() == FALSE){
			printf("Wrong format\n");
			return 0;
		}
		printf("Correct format\n");
		rewind(input1);
		process();
		fclose(input1);
	}
	//Verbose mode
	else if(argc == 3){

		input1 = fopen(argv[2],"r");
		printf("%s\n", argv[1]);

		if(input1 == NULL)
			return 0;
		else if ( strcmp(argv[1], "-v") == 0){
			//process();
			printf("Verbose mode and file is not null \n");
			if (checkFile() == FALSE){
				printf("Wrong format\n");
				return 0;
			}
			printf("Correct format\n");
			rewind(input1);
			process();
			fclose(input1);
		}
		//when argument is not "-v"
		else{
			fprintf(stderr, "-V is expected on the 2nd argument %s\n", argv[1]);
			return 0;
		}
	}

	return 0;
}



Boolean checkFile(){
	char temp;
	Boolean EPcount = FALSE;
	int count = 1;

	while( (temp = getc(input1)) != EOF ){
		
		//Check the first line
		if(EPcount == FALSE){

			// printf("%c\n", temp);

			if( temp != 'E')
				return FALSE;

			if ((temp = getc(input1)) != 'P')
				return FALSE;

			// printf("%c\n", temp);

			if ( (temp = getc(input1)) != ':')
				return FALSE;

			// printf("%c\n", temp);

			if ( (temp = getc(input1)) != ' ')
				return FALSE;
			
			// printf("%c\n", temp);

			//Next 3instructions code
			temp = getc(input1);
			// printf("%c\n", temp);
			if( check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			// printf("%c\n", temp);
			if(check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			// printf("%c\n", temp);
			if( check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			if( (temp == '\n') || (temp == EOF) )
				;
			else
				return FALSE;
			// printf("%c lol\n", temp);

			EPcount = TRUE;
		}
		else{

			// printf("%c\n", temp);

			if( (count % 10) == 4){
				if (temp != ':')
					return FALSE;
			}
			else if ((count % 10) == 5){
				if(temp != ' ')
					return FALSE;
			}
			else if((count % 10) == 9){
				if( (temp == '\n') || (temp== EOF) )
					;
				else
					return FALSE;
			}
			else{
				//printf("enter \n");
				if ( check_char_within_range(temp) == 0 )
					return FALSE;
			}


			if(count == 9)
				count = 1;
			else
				count++;
		}
	}

	return TRUE;
}



void process(){
	load_into_memory();
	int debug = 0;

	while (HALT == FALSE ){
		instructions = (STRING) malloc( (sizeof(char)) * 29 );

		decide_instruction_sets();
		free(instructions);

		program_counter++;

		if(skipFlag == TRUE)
			program_counter++;


		if(debug == 7)
			break;
		else
			debug++;
	}
}



Boolean check_char_within_range(char receivedChar){
	if( (receivedChar >= 48 && receivedChar <= 57 )
		|| (receivedChar >= 65 && receivedChar <= 70 ) )
		return TRUE;

	return FALSE;
}



void load_into_memory(){
	int temp;
	short index_hex2dec = 0;
	short instruct_hex2dec = 0;

	int i;
	int k;

	int count = 1;

	int countTemp = 1;
	int countInstruction = 1;

	for(i = 0; i < 8; i++){
		temp = getc(input1);

		if( i >= 4 && i <= 6){
			printf("%c\n", temp);
			temp = convert_A_to_F_num(temp);
			temp -= 48;

			printf("cool\n");

			if(count == 1)
				temp <<= 8;
			else if (count == 2)
				temp <<= 4;

			//Get Program counter
			program_counter += temp;
			count++;
		}
	}

	//Reserve for future use
	count = 0;

	printf("program counter %d\n", program_counter);
	printf("%c This space\n", temp);
	printf("ccoool stuff starts here\n");
	while( (temp = getc(input1)) != EOF){
		if (count <= 3){
			temp = convert_A_to_F_num(temp);
			temp -= 48;

			if(count == 1)
				temp <<= 8;
			else if (count == 2)
				temp <<= 4;

			index_hex2dec += temp;
		}

		if( (countInstruction % 10) >= 6 && (countInstruction % 10) <= 8 ){
			temp = convert_A_to_F_num(temp);
			temp -= 48;
			temp &= 0xF;
			temp <<= ( (3-countTemp) * 4 );
			instruct_hex2dec |= temp;

			instruct_hex2dec &= 0xFFF;
			countTemp++;
		}

		//increment
		count++;

		//Reset or increment
		if(countInstruction == 9){
			my_memory[index_hex2dec] = instruct_hex2dec;

			index_hex2dec = 0;
			instruct_hex2dec = 0;

			countInstruction = 1;
			countTemp = 1;

			count = 1;
		}
		else
			countInstruction++;

	}

	for(k = 0; k < 4096; k++){
		if (my_memory[k] != 0){
			printf("index: %d  %d\n", k, my_memory[k]);
		}
	}

}



char convert_A_to_F_num(char temp){
	switch(temp){
		case 'A':
			return 58;
		case 'B':
			return 59;
		case 'C':
			return 60;
		case 'D':
			return 61;
		case 'E':
			return 62;
		case 'F':
			return 63;
		default:
			return temp;
	}
}



void decide_instruction_sets(){
	int opcode = my_memory[program_counter];

	opcode >>= 9;
	opcode &= 0x7;

	//printf("\n Whaat is this opcode : %d\n", opcode);

	switch(opcode){
		case 0:
			strcat(instructions, "AND");
			first_fetch_sets();
			printTheResult();	
			break;
		case 1:
			strcat(instructions, "TAD");
			printf("%s\n", instructions);
			first_fetch_sets();
			printTheResult();
			break;
		case 2:
			strcat(instructions, "ISZ");
			first_fetch_sets();
			printTheResult();
			break;
		case 3:
			strcat(instructions, "DCA");
			first_fetch_sets();
			printTheResult();
			break;
		case 4:
			strcat(instructions, "JMS");
			first_fetch_sets();
			printTheResult();
			break;
		case 5:
			strcat(instructions, "JMP");
			first_fetch_sets();
			printTheResult();
			break;
		case 7: //111
			second_fetch_sets();
			printTheResult();
			break;
		case 6: //110
			third_fetch_sets();
			printTheResult();
			break;
	}
}



void first_fetch_sets(){
	int effectiveAddress = compute_effectiveAddr();
	int opcode = my_memory[program_counter];

	_instruct = my_memory[program_counter]; 

	//printf("%03X\n", _instruct);

	opcode >>= 9;

	switch(opcode){
		case 0:
			registerA &= effectiveAddress;
			cycle_count+=2;
			break;
		case 1:
			registerA |= effectiveAddress;
			registerA &= 0xFFF;
			cycle_count+=2;
			break;
		case 2:
			effectiveAddress++;
			my_memory[effectiveAddress] = effectiveAddress;
			
			//Result zero
			if(my_memory[effectiveAddress] == 0)
				program_counter++;

			cycle_count+=2;
			break;
		case 3:
			registerA = effectiveAddress;
			registerA = 0;
			my_memory[program_counter] = 0;
			cycle_count+=2;
			break;
		case 4:
			effectiveAddress = program_counter++;
			program_counter = effectiveAddress++;
			cycle_count+=2;
			break;
		case 5:
			program_counter = effectiveAddress;
			cycle_count++;
			break;
	}

}



int compute_effectiveAddr(){
	int effectiveAddress = 0;
	int checkingZC = 0;
	int checkingDI = 0;
	int mask = 0;
	//Process effective address
	checkingZC = my_memory[program_counter];
	checkingZC >>= 7;
	checkingZC &= 0x1;

	mask = 1 << 11 >> 4;
	if (checkingZC == 0){
		effectiveAddress = my_memory[program_counter] & (~mask);
	}
	else if(checkingZC == 1){
		effectiveAddress = (program_counter & mask);
		effectiveAddress |= (my_memory[program_counter] & (~mask) );
	}

	//addr:      000001101001
	//content:   001001110101
	//					10001
	//			 001001110101

	checkingDI = my_memory[program_counter];
	checkingDI >>= 8;
	checkingDI &= 0x1;

	if(checkingDI == 1)
		effectiveAddress = my_memory[effectiveAddress];

	if(effectiveAddress != 0)
		printf("This is cool %d\n", effectiveAddress);
	
	return effectiveAddress;
}



void second_fetch_sets(){
	int checkGroup = my_memory[program_counter];
	
	int CLA = 	my_memory[program_counter];
	int CLL = 	my_memory[program_counter];
	int CMA = 	my_memory[program_counter];
	int CML = 	my_memory[program_counter];
	int RAR = 	my_memory[program_counter];
	int RAL = 	my_memory[program_counter];
	int rotate = my_memory[program_counter];
	int IAC = 	my_memory[program_counter];

	int SMA = my_memory[program_counter];
	int SZA = my_memory[program_counter];
	int SNL = my_memory[program_counter];
	int RSS = my_memory[program_counter];
	int OSR = my_memory[program_counter];
	int HLT = my_memory[program_counter];
	//Swap for Rotation
	int swap;

	//Update the instruction view
	_instruct = my_memory[program_counter];

	//Get the Group No.
	checkGroup >>=8;
	checkGroup &= 0x1;

	CLA >>= 7;
	CLA &= 0x1;
	
	//Group 1
	if(checkGroup == 0){


		CLL >>=6;
		CLL &= 0x1;

		CMA >>=5;
		CMA &= 0x1;

		CML >>=4;
		CML &= 0x1;

		RAR >>=3;
		RAR &= 0x1;

		RAL >>=2;
		RAL &= 0x1;

		rotate >>=1;
		rotate &= 0x1;

		IAC &= 0x1;

		if (CLA == 1){
			//Clear both the A register
			registerA = 0;
			strcat(instructions, "CLA");
		}

		if (CLL == 1){
			//Clear the Link bit
			linkBit = 0;
			strcat(instructions, "CLL");
		}
		
		if(CMA == 1){
			//Complement the A register (bit by bit, change 1 to 0 and 0 to 1).
			registerA = ~registerA;
			strcat(instructions, "CMA");
		}
		
		if (CML == 1){
			//Complement the Link bit. 
			linkBit = ~linkBit;
			strcat(instructions, "CML");
		}
		
		if (IAC == 1){
			//two's complement
			registerA++;
			strcat(instructions, "IAC");
		}
		
		if (RAR == 1){
			//Rotate the A register right

			//1bit
			if(rotate == 0){
				swap = registerA;
				registerA >>= 1;
				registerA |= (linkBit << 11);
				linkBit = swap  & 0x1;
			}
			//2bit
			else{
				swap = registerA;
				registerA >>= 1;
				registerA |= (linkBit << 11);
				linkBit = swap  & 0x1;

				swap = registerA;
				registerA >>= 1;
				registerA |= (linkBit << 11);
				linkBit = swap  & 0x1;
			}

			strcat(instructions, "RAR");
		}
		
		if (RAL == 1){
			//Rotate the A register left.
			//1bit
			if(rotate == 0){
				swap = registerA;
				registerA <<= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;
			}
			//2bit
			else{
				swap = registerA;
				registerA <<= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;

				swap = registerA;
				registerA <<= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;
			}
		}
		strcat(instructions, "RAL");
	}


	//Group 2
	if (checkGroup == 1 ){
		SMA >>=6;
		SMA &= 0x1;

		SZA >>=5;
		SZA &= 0x1;

		SNL >>=4;
		SNL &= 0x1;

		RSS >>=3;
		RSS &= 0x1;

		OSR >>=2;
		OSR &= 0x1;

		HLT >>=1;
		HLT &= 0x1;

		//How about the last bit
		//Always 0?
		
		//Skip on Minus
		if (SMA == 1){
			if (  ((registerA >> 11) & 0x1) == 1 )
				skipFlag = TRUE;

			strcat(instructions, "SMA");
		}

		//Skip on Zero
		if(SZA == 1){
			if (registerA == 0){
				skipFlag = TRUE;
			}

			strcat(instructions, "SZA");
		}

		//Skip on Nonzero Link.
		if(SNL == 1){
			if (linkBit == 1){
				skipFlag = TRUE;
			}

			strcat(instructions, "SNL");
		}


		//If none are selected, the Skip flag is cleared
		if ( (SMA | SZA | SNL) == 0 )
			skipFlag = FALSE;


		//Reverse Skip Sense
		if(RSS == 1){
			skipFlag = !skipFlag;

			strcat(instructions, "RSS");
		}

		//Clear both the A register
		if (CLA == 1){
			registerA = 0;

			strcat(instructions, "CLA");
		}

		//Treat it as NOP
		//But how?
		if(OSR == 1){
		}

		if(HLT == 1){
			HALT = TRUE;

			strcat(instructions, "HLT");
		}
	}
}



void third_fetch_sets(){
	//If the device is 3, do a getchar() to read a character from standard input and put that character in the A register. 
	//If the device is 4, take the low-order 8 bits from the A register and output it as an ASCII character to standard output (putchar()). 
	//Assume these IOT instructions take 1 cycle.
	int instruction = my_memory[program_counter];
	int device = my_memory[program_counter];
	int function3bits = my_memory[program_counter];

	device >>= 3;
	device &= 0x3F;

	function3bits &= 0x7;

	if(device == 3){
		registerA = getc(stdin);
	}
	else if (device == 4){
		putchar(registerA & 0xFF);
		//fprintf(stderr, "Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n", ...);
	}
	else{
		HALT = TRUE;
	}

}



void printTheResult(){

	printf("Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n", 
			cycle_count, program_counter, _instruct, instructions, registerA, linkBit);
}



void instruction_cat(STRING thisInstruction){




}