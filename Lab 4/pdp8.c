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
int cycle_count = 0;


int registerA = 0;
int linkBit = 0;
Boolean skipFlag = FALSE;

Boolean checkFile();
Boolean check_char_within_range(char receivedChar);

//Controller
void process();

//Load Memory
void load_into_memory();
char convert_A_to_F_num(char temp);

//Fetching
Boolean decide_instruction_sets();

int compute_effectiveAddr();
void first_fetch_sets();
Boolean second_fetch_sets();
void third_fetch_sets();



int main(int argc, char **argv){
	Boolean verbose = TRUE;

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


Boolean check_char_within_range(char receivedChar){
	if( (receivedChar >= 48 && receivedChar <= 57 )
		|| (receivedChar >= 65 && receivedChar <= 70 ) )
		return TRUE;

	return FALSE;
}



void process(){
	load_into_memory();
	if (decide_instruction_sets() == FALSE){
		program_counter++;
	}

	if (decide_instruction_sets() == FALSE){
		program_counter++;
	}

	if (decide_instruction_sets() == FALSE){
		program_counter++;
	}

	if (decide_instruction_sets() == FALSE){
		program_counter++;
	}

	if (decide_instruction_sets() == FALSE){
		program_counter++;
	}
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


Boolean decide_instruction_sets(){
	int opcode = program_counter;
	Boolean HALT = FALSE;

	opcode >>= 9;
	opcode &= 0xF;

	switch(opcode){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			first_fetch_sets();
			break;
		case 7: //111
			if (second_fetch_sets())
				HALT = TRUE;
			break;
		case 6: //110
			third_fetch_sets();
			break;
	}

	return HALT;
}



void first_fetch_sets(){
	int effectiveAddress = compute_effectiveAddr();
	int insturction = my_memory[program_counter];
	int opcode = my_memory[program_counter]; 

	opcode >>= 9;

	switch(opcode){
		case 0:
			registerA &= effectiveAddress;
			cycle_count+=2;
			break;
		case 1:
			registerA += effectiveAddress;
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
		effectiveAddress = my_memory[program_counter] & ~mask;
	}
	else if(checkingZC == 1){
		effectiveAddress = (program_counter & mask);
		effectiveAddress |= (my_memory[program_counter] & ~mask);
	}

	checkingDI = my_memory[program_counter];
	checkingDI >>= 8;
	checkingDI &= 0x1;

	if(checkingDI == 1)
		effectiveAddress = my_memory[effectiveAddress];

	printf("This is cool %d\n", effectiveAddress);
	return effectiveAddress;
}



Boolean second_fetch_sets(){
	Boolean HALT = FALSE;
	int insturction = my_memory[program_counter];
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
		}

		if (CLL == 1){
			//Clear the Link bit
			linkBit = 0;
		}
		
		if(CMA == 1){
			//Complement the A register (bit by bit, change 1 to 0 and 0 to 1).
			registerA = ~registerA;
		}
		
		if (CML == 1){
			//Complement the Link bit. 
			linkBit = ~linkBit;
		}
		
		if (IAC == 1){
			//two's complement
			registerA++;
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
		}
		
		if (RAL == 1){
			//Rotate the A register left.
			//1bit
			if(rotate == 0){
				swap = registerA;
				registerA <= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;
			}
			//2bit
			else{
				swap = registerA;
				registerA <= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;

				swap = registerA;
				registerA <= 11;
				registerA |= linkBit;
				linkBit = (swap >> 11)  & 0x1;
			}
		}

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
		}

		//Skip on Zero
		if(SZA == 1){
			if (registerA == 0){
				skipFlag = TRUE;
			}
		}

		//Skip on Nonzero Link.
		if(SNL == 1){
			if (linkBit == 1){
				skipFlag = TRUE;
			}
		}


		//If none are selected, the Skip flag is cleared
		if ( (SMA | SZA | SNL) == 0 )
			skipFlag = FALSE;


		//Reverse Skip Sense
		if(RSS == 1){
			skipFlag = !skipFlag;
		}

		//Clear both the A register
		if (CLA == 1){
			registerA = 0;
		}

		//Treat it as NOP
		//But how?
		if(OSR == 1){
		}

		if(HLT == 1)
			HALT = TRUE;
	}


	return HALT;
}


void third_fetch_sets(){

}