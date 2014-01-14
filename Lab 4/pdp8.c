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
int updatePC = 0;
int _instruct = 0;
long long cycle_count = 0;

STRING instructions;

int registerA = 0;
int linkBit = 0;
Boolean skipFlag = FALSE;
Boolean IOTFlag = FALSE;
Boolean HALT = FALSE;

Boolean nextPCUpdate;
Boolean JMSUpdate;


//Functions Prototype
Boolean checkFile();
Boolean checkEP(STRING bufferString);
Boolean checkNonEP(STRING bufferString);
Boolean check_char_within_range(char receivedChar);

//Controller
void process();

//Load Memory
void load_into_memory();
int char_to_hex(STRING temp);

//Fetching
void decide_instruction_sets();

int compute_effectiveAddr();
void first_fetch_sets();
void second_fetch_sets();
void third_fetch_sets();


//Printing the result
void printTheResult();
void remove_trailer_space();


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
	Boolean EP_exists = FALSE;
	int i = 0;

	STRING bufferString = (STRING) malloc( (sizeof(char)) * 10 );

	//Multiple EP case
	while( fgets(bufferString, 10, input1) != NULL ){

		if (checkNonEP(bufferString) == FALSE){
			
			if( checkEP(bufferString) == TRUE ){
				EP_exists = TRUE;
			}
			else
				break;
		}
	}

	bufferString[0] = '\0';
	free(bufferString);

	return EP_exists;
}



Boolean checkEP(STRING bufferString){
	char temp[3];
	int i;

	if( (bufferString[0] == 'E') && 
		(bufferString[1] == 'P') &&
		(bufferString[2] == ':') &&
		(bufferString[3] == ' ') &&
		( check_char_within_range(bufferString[4]) ) &&
		( check_char_within_range(bufferString[5]) ) &&
		( check_char_within_range(bufferString[6]) )    ){

		for(i = 4; i < 7 ; i++){
			temp[i-4] = bufferString[i];
		}

		//printf("SSS  %s\n", temp);
		program_counter = char_to_hex(temp);

		//printf("LAlll %lx\n",x );
		//printf("LAlll %ld\n",x );

		return TRUE;
	}


	return FALSE;
}



Boolean checkNonEP(STRING bufferString){
	int i = 1;

	while(i < 10){

		if( ((i >= 1) && (i <= 3)) || 
			((i >= 6) && (i <= 8))  ){
			if( check_char_within_range(bufferString[ i - 1]) == FALSE ){
				return FALSE;
			}
		}
		else if (i == 4){
			if( bufferString[ i - 1 ] != ':' ){
				return FALSE;
			}
		}
		else if (i == 5){
			if (bufferString[ i - 1 ] != ' '){
				return FALSE;
			}
		}
		else if (i == 9){

			if(bufferString [ i - 1] == EOF){
				return TRUE;
			}

			if(bufferString [ i - 1 ] != '\n'){
				return FALSE;
			}
		}

		i++;
	}

	return TRUE;
}



Boolean check_char_within_range(char receivedChar){
	if( (receivedChar >= 48 && receivedChar <= 57 )
		|| (receivedChar >= 65 && receivedChar <= 70 ) ){
		return TRUE;
	}
	
	return FALSE;
}



void load_into_memory(){
	int printIndex;
	int real_address;
	int real_instruct_mem;

	STRING bufferString = (STRING) malloc( (sizeof(char)) * 10 );
	char memAddress[3];
	char instructMem[3];

	int i;

	//load into memory
	while( fgets(bufferString, 10, input1) != NULL ){

		if(checkEP(bufferString) == FALSE){
			
			for(i = 0; i < 3 ; i++){
				memAddress[i] = bufferString[i];
			}

			real_address = char_to_hex(memAddress);

			
			for(i = 5; i < 8 ; i++){
				instructMem[i-5] = bufferString[i];
			}

			printf("AAA%sAAA\n", instructMem);
			real_instruct_mem= char_to_hex(instructMem);

			my_memory[real_address] = real_instruct_mem & 0xFFF;
		}

	}


	bufferString[0] = '\0';
	free(bufferString);



	//Print to check stuff inside memory -debugmem
	for(printIndex = 0; printIndex < 4096; printIndex++){
		if (my_memory[printIndex] != 0){
			printf("index: %x  %x\n", printIndex, my_memory[printIndex]);
		}
	}

}



int char_to_hex(STRING temp){
	int hex_now = 0;
	int i;

	for(i = 0; i < 3; i++){

		if( (temp[i] >= '0') && (temp[i] <= '9') )
			temp[i] -= '0';
		else
			temp[i] -= 55;

		if(i == 0){
			hex_now |= (temp[i] << 8);
		}

		if(i == 1){
			hex_now |= (temp[i] << 4);
		}
	}

	hex_now |= temp[2];
	return hex_now;
}



void process(){
	load_into_memory();
	int debug = 0;

	while (HALT == FALSE ){
		instructions = (STRING) malloc( (sizeof(char)) * 29 );

		//Get whatever the instruction set
		decide_instruction_sets();

		printTheResult();

		//Check whether skip flag is on
		if(skipFlag == TRUE){
			program_counter = (program_counter + 1) % 4096;
		}

		//Every Execution must increase program counter
		program_counter = (program_counter + 1) % 4096;

		//Null the string before freeing it
		instructions[0] = '\0';
		free(instructions);

		if(nextPCUpdate == TRUE){
			program_counter = updatePC;
			nextPCUpdate = FALSE;
		}

		if(JMSUpdate == TRUE){
			program_counter = updatePC;
			JMSUpdate = FALSE;
		}

		//Debug
		// if(debug == 10)
		// 	break;
		// else
		// 	debug++;

		skipFlag = FALSE;
	}
}



void decide_instruction_sets(){
	int opcode = my_memory[program_counter];

	opcode >>= 9;
	opcode &= 0x7;

	//printf("\n Whaat is this opcode : %d\n", opcode);

	switch(opcode){
		case 0:
			strcat(instructions, "AND ");
			first_fetch_sets();
			//printTheResult();	
			break;
		case 1:
			//printf("Effective %x\n", compute_effectiveAddr());
			strcat(instructions, "TAD ");
			first_fetch_sets();
			//printTheResult();
			break;
		case 2:
			strcat(instructions, "ISZ ");
			first_fetch_sets();
			//printTheResult();
			break;
		case 3:
			strcat(instructions, "DCA ");
			first_fetch_sets();
			//printTheResult();
			break;
		case 4:
			strcat(instructions, "JMS ");
			first_fetch_sets();
			//printTheResult();
			break;
		case 5:
			strcat(instructions, "JMP ");
			first_fetch_sets();
			//printTheResult();
			break;
		case 7: //111
			second_fetch_sets();
			//printTheResult();
			break;
		case 6: //110
			third_fetch_sets();
			//printTheResult();
			break;
	}
}



void first_fetch_sets(){
	int effectiveAddress = compute_effectiveAddr();
	int opcode = my_memory[program_counter];
	int signBit = 0;

	_instruct = my_memory[program_counter]; 

	//printf("%03X\n", _instruct);

	opcode >>= 9;

	//printf("%d\n", my_memory[10]);
	switch(opcode){
		case 0: //AND
			registerA &= my_memory[effectiveAddress];
			registerA &= 0xFFF;
			cycle_count+=2;
			break;
		case 1: //TAD
			registerA += my_memory[effectiveAddress];

			//A carry out of the high-order bit (sign bit) 
			//will complement the Link bit.
			signBit = registerA;
			signBit >>= 12;
			signBit &= 0x1;

			registerA %= 4096;

			if ( signBit == 1){
				linkBit = ~linkBit;
				linkBit &= 0x1;
			}
			registerA &= 0xFFF;
			cycle_count+=2;
			break;
		case 2:
			my_memory[effectiveAddress] +=1;
			my_memory[effectiveAddress] %= 4096;

			//printf("This %d\n", my_memory[effectiveAddress]);
			//Result zero
			if(my_memory[effectiveAddress] == 0){
				updatePC = program_counter + 2;
				updatePC %= 4096;
				nextPCUpdate = TRUE;
			}

			cycle_count+=2;
			break;
		case 3:
			my_memory[effectiveAddress] = registerA;
			registerA = 0;
			registerA &= 0xFFF;
			my_memory[program_counter] = 0;
			cycle_count+=2;
			break;
		case 4:
			my_memory[effectiveAddress] = program_counter + 1;
			updatePC = effectiveAddress + 1;
			JMSUpdate = TRUE;
			cycle_count+=2;
			break;
		case 5:
			//printf("PRogram counter %d %d\n", program_counter, effectiveAddress);
			//hack update program counter after everything
			updatePC = effectiveAddress;
			nextPCUpdate = TRUE;
			
			cycle_count++;
			break;
	}

}



int compute_effectiveAddr(){
	int effectiveAddress = 0;
	int checkingZC = 0;
	int checkingDI = 0;
	int mask = 0;
	int mask2 = 0;
	//Process effective address
	checkingZC = my_memory[program_counter];
	checkingZC >>= 7;
	checkingZC &= 0x1;


	mask = 0xF80; //111110000000
	mask2 = 0x7F; //000001111111
	//printf("%x\n", mask);

	if (checkingZC == 0){
		//printf("ZC is zero\n");
		effectiveAddress = my_memory[program_counter] & mask2;
	}
	else if(checkingZC == 1){
		//printf("ZC is one\n");
		effectiveAddress = (program_counter & mask);
		effectiveAddress = effectiveAddress | ( my_memory[program_counter] & mask2 );
	}


	checkingDI = my_memory[program_counter];
	checkingDI >>= 8;
	checkingDI &= 0x1;


	if(checkingDI == 1)
		effectiveAddress = my_memory[effectiveAddress];

	//if(effectiveAddress != 0)	
	return effectiveAddress;
}



void second_fetch_sets(){
	int checkGroup = my_memory[program_counter];
	
	int CLA = 	my_memory[program_counter];
	int CLL = 	my_memory[program_counter];
	int CMA = 	my_memory[program_counter];
	int CML = 	my_memory[program_counter];
	int RTR = 	my_memory[program_counter];
	int RTL = 	my_memory[program_counter];
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

	int signBit = 0;

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

		RTR >>=3;
		RTR &= 0x1;

		RTL >>=2;
		RTL &= 0x1;

		rotate >>=1;
		rotate &= 0x1;

		IAC &= 0x1;

		if (CLA == 1){
			//Clear both the A register
			registerA = 0;
			registerA &= 0xFFF;
			strcat(instructions, "CLA ");
		}

		if (CLL == 1){
			//Clear the Link bit
			linkBit = 0;
			linkBit &= 1;
			strcat(instructions, "CLL ");
		}
		
		if(CMA == 1){
			//Complement the A register (bit by bit, change 1 to 0 and 0 to 1).
			registerA = ~registerA;
			registerA &= 0xFFF;
			strcat(instructions, "CMA ");
			//cycle_count++;
		}
		
		if (CML == 1){
			//Complement the Link bit. 
			linkBit = ~linkBit;
			linkBit &= 0x1;
			strcat(instructions, "CML ");
		}
		
		if (IAC == 1){
			//two's complement
			registerA++;

			signBit = registerA;
			signBit >>= 12;
			signBit &= 0x1;

			registerA %= 4096;

			if ( signBit == 1){
				linkBit = ~linkBit;
				linkBit &= 0x1;
			}

			registerA &= 0xFFF;
			strcat(instructions, "IAC ");
			//cycle_count++;
		}
		
		if (RTR == 1){
			//Rotate the A register right

			//1bit
			if(rotate == 0)
				strcat(instructions, "RAR ");

			swap = registerA;
			registerA >>= 1;
			registerA |= (linkBit << 11);
			registerA &= 0xFFF;
			linkBit = swap  & 0x1;
			
			//2bit
			if(rotate ==1){
				swap = registerA;
				registerA >>= 1;
				registerA |= (linkBit << 11);
				registerA &= 0xFFF;
				linkBit = swap  & 0x1;

				strcat(instructions, "RTR ");
			}

			
		}
		
		if (RTL == 1){
			//Rotate the A register left.
			//1bit or 2 bit(first rotate)
			if(rotate == 0)
				strcat(instructions, "RAL ");
			
			swap = registerA;
			registerA <<= 1;
			registerA |= linkBit;
			registerA &= 0xFFF;
			linkBit = (swap >> 11)  & 0x1;
			
			//2bit (second rotate)
			if (rotate == 1){
				swap = registerA;
				registerA <<= 1;
				registerA |= linkBit;
				registerA &= 0xFFF;
				linkBit = (swap >> 11)  & 0x1;

				strcat(instructions, "RTL ");
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

			strcat(instructions, "SMA ");
		}

		//Skip on Zero
		if(SZA == 1){
			if (registerA == 0){
				skipFlag = TRUE;
			}

			strcat(instructions, "SZA ");
		}

		//Skip on Nonzero Link.
		if(SNL == 1){
			if (linkBit == 1){
				skipFlag = TRUE;
			}

			strcat(instructions, "SNL ");
		}


		//If none are selected, the Skip flag is cleared
		if ( !(SMA + SZA + SNL) )
			skipFlag = FALSE;

		//Reverse Skip Sense
		if(RSS == 1){
			skipFlag = !skipFlag;

			strcat(instructions, "RSS ");
		}

		//printf("What is this skip flag  %d\n", skipFlag);
		//Clear both the A register
		if (CLA == 1){
			registerA = 0;
			registerA &= 0xFFF;

			strcat(instructions, "CLA ");
		}

		//Treat it as NOP
		//But how?
		if(OSR == 1){
			strcat(instructions, "NOP ");
		}

		if(HLT == 1){
			HALT = TRUE;

			strcat(instructions, "HLT ");
		}
	}

	cycle_count++;
}



void third_fetch_sets(){
	//If the device is 3, do a getchar() to read a character from standard input and put that character in the A register. 
	//If the device is 4, take the low-order 8 bits from the A register and output it as an ASCII character to standard output (putchar()). 
	//Assume these IOT instructions take 1 cycle.
	int instruction = my_memory[program_counter];
	int device = my_memory[program_counter];
	int function3bits = my_memory[program_counter];

	//Update for IOT
	_instruct = my_memory[program_counter];

	device >>= 3;
	device &= 0x3F;

	function3bits &= 0x7;

	if(device == 3){
		registerA = getchar();
		registerA &= 0xFFF;

		strcat(instructions, "IOT 3 ");
	}
	else if (device == 4){
		putchar(registerA & 0x0FF);
		strcat(instructions, "IOT 4 ");
		//fprintf(stderr, "Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n", ...);
	}
	else{
		HALT = TRUE;
	}

}



void printTheResult(){
	remove_trailer_space();

	fprintf(stderr,"Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n", 
			cycle_count, program_counter, _instruct, instructions, registerA, linkBit);
}



void remove_trailer_space(){
	int lastchar = strlen(instructions);
	instructions[lastchar -1 ] = '\0';
}