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

static int memory[4096];
int program_counter = 0;
int cycle_count = 0;

Boolean checkFile();
Boolean check_char_within_range(char receivedChar);
void process();




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
		//process();
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
			fclose(input1);
		}
		//when argument is not "-v"
		else{
			fprintf(stderr, "-V is expected on the 2nd argument\n", argv[1]);
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

			printf("%c\n", temp);

			if( temp != 'E')
				return FALSE;

			if ((temp = getc(input1)) != 'P')
				return FALSE;

			printf("%c\n", temp);

			if ( (temp = getc(input1)) != ':')
				return FALSE;

			printf("%c\n", temp);

			if ( (temp = getc(input1)) != ' ')
				return FALSE;
			
			printf("%c\n", temp);

			//Next 3instructions code
			temp = getc(input1);
			printf("%c\n", temp);
			if( check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			printf("%c\n", temp);
			if(check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			printf("%c\n", temp);
			if( check_char_within_range(temp) == FALSE )
				return FALSE;

			temp = getc(input1);
			if( (temp == '\n') || (temp == EOF) )
				;
			else
				return FALSE;
			printf("%c lol\n", temp);

			EPcount = TRUE;
		}
		else{

			printf("%c\n", temp);

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

			// if ( temp == 'h'){
			// 	printf("%d\n", count);
			// 	printf("This boolean %d\n", (check_char_within_range(temp)));
			// }
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
	

}
