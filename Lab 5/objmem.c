/* 
   Assembler for PDP-8.  Memory and object file creation. 
*/

#include "asm8.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we want to assemble instructions.  We could assemble and output them
   all at once.  But we have a problem of forward references.  So we
   keep the (partially) assembled instructions in an array of them,
   essentially simulated memory.  That allows us to come back and 
   fix them up when the forward reference is resolved.

   We need to know which memory locations are from assembled
   instructions, and which are just empty; so each memory location
   has a bit (defined/not defined).
*/

INST     memory[4096];
Boolean defined[4096];
Address entry_point = 0;


int get_the_block(int startIndex);
void write_the_block(int startIndex, int endIndex);
void need_newline_or_not();
void need_space_or_not();


void Clear_Object_Code(void)
{
    int i;
    for (i = 0; i < 4096; i++)
        {
            defined[i] = FALSE;
        }
}

void Define_Object_Code(Address addr, INST inst, Boolean redefine)
{
    if (debug)
        fprintf(stderr, "object code: 0x%03X = 0x%03X\n", addr, inst);
    if (defined[addr] && !redefine)
        {
            fprintf(stderr, "redefined memory location: 0x%03X: was 0x%03X; new value 0x%03X\n",
                    addr, memory[addr], inst);
            number_of_errors += 1;
        }
                
    defined[addr] = TRUE;
    memory[addr] = inst;
}

INST Fetch_Object_Code(Address addr)
{
    INST inst;

    if (defined[addr])
        inst = memory[addr];
    else
        inst = 0;

    if (debug)
        fprintf(stderr, "read object code: 0x%03X = 0x%03X\n", addr, inst);
    return(inst);
}

FILE * pFile;
int howManyBytes = 0;

void Output_Object_Code(void)
{
  
  pFile = fopen("abc.obj", "w");

  int byte_5;
  int byte_6;

  fprintf(pFile, "4f42 4a38 ");
  
  byte_5 = entry_point >> 6 & 0x3F;
  byte_6 = entry_point & 0x3F;

  //fprintf(stderr, "lalalal  %x\n", entry_point);
  fprintf(pFile, "%02x", byte_5);
  fprintf(pFile, "%02x", byte_6);

  howManyBytes +=12;
  need_space_or_not();

  int i;
  int endIndex;
  int totalblock;
  int block_1;

  for (i = 0; i < 4096; i++){
    if (defined[i]){
      endIndex = get_the_block(i);

      totalblock = endIndex - i;
      totalblock += 1;
      totalblock *= 2;
      totalblock += 1;

      // if(totalblock ==9)
      //   fprintf(stderr,"%d\n", totalblock);
      //Write size of block
      block_1 = totalblock & 0x3F;

      fprintf(pFile, "%02x", block_1);
      howManyBytes+=2;
      need_space_or_not();
      need_newline_or_not();
      
      write_the_block(i, endIndex);

      i = endIndex;
    }



  }
  
  printf("%d\n", howManyBytes);
  fclose(pFile);
}



int get_the_block(int startIndex){
  
  int i = startIndex;
  int max = startIndex + 253;

  while( i <= max && defined[i]){
    i++;
  }

  return i;
}


void write_the_block(int startIndex, int endIndex){
  
  int i = startIndex;

  int addr_1;
  int addr_2;

  int val_1;
  int val_2;

  while( i < endIndex){

    //Write the address
    if(i == startIndex){
      addr_1 = startIndex >> 6 & 0x3F;
      addr_2 = startIndex & 0x3F;
      fprintf(pFile, "%02x", addr_1);
      howManyBytes+=2;
      need_space_or_not();
      need_newline_or_not();

      fprintf(pFile, "%02x", addr_2);
      howManyBytes+=2;
      need_space_or_not();
      need_newline_or_not();
    }
   

      val_1 = (memory[i] >> 6) & 0x3F;
      val_2 = memory[i] & 0x3F;
      
      fprintf(pFile, "%02x", val_1);
      howManyBytes+=2;
      need_space_or_not();
      need_newline_or_not();

      fprintf(pFile, "%02x", val_2);
      howManyBytes+=2;
      need_space_or_not();
      need_newline_or_not();
    

    i++;
  }

}


void need_newline_or_not(){

  if ( (howManyBytes % 32) == 0 ){
      fprintf(pFile, "\n");
    }

}


void need_space_or_not(){

  if ( (howManyBytes % 4) == 0 && ((howManyBytes % 32) != 0) ){
      fprintf(pFile, " ");
    }

}


