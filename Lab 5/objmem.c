//Name   : Kim Yu Ng
//CS ID  : kimyu92
//UT EID : kn6254
//Section: 53835
//TA     : Ji Hong
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


int howManyBytes = 0;
Boolean last = FALSE;


void Output_Object_Code(void)
{

  int obj_1 = 0x4f;
  int obj_2 = 0x42;
  int obj_3 = 0x4a;
  int obj_4 = 0x38;

  int byte_5;
  int byte_6;

  fprintf(pFile, "%c", obj_1);
  fprintf(pFile, "%c", obj_2);
  fprintf(pFile, "%c", obj_3);
  fprintf(pFile, "%c", obj_4);
  
  byte_5 = (entry_point >> 6) & 0x3F;
  byte_6 = entry_point & 0x3F;

  fprintf(pFile, "%c", byte_5);
  fprintf(pFile, "%c", byte_6);

  howManyBytes +=12;

  int i;
  int endIndex;
  int totalblock;
  int block_1;
  int balance;

  for (i = 0; i < 4096; i++){
    if (defined[i]){
      endIndex = get_the_block(i);

      //Calculate block
      totalblock = endIndex - i; //end - start    eg 5 -0 = 5
      totalblock += 1; //plus one back            the ans is 6
      totalblock *= 2; //two bytes
      totalblock += 1; //block itself 0xff is max

      //total = max
      if (totalblock > 0xff ){
        balance = totalblock - 0xff;

        totalblock = 0xff;
        endIndex -= balance;
        last = TRUE;
      }
      
      

      block_1 = totalblock;

      fprintf(pFile, "%c", block_1);
      howManyBytes+=2;

      //Passing the index
      write_the_block(i, endIndex);

      i = endIndex;
    }
  } 
}



int get_the_block(int startIndex){
  
  int i = startIndex;

  while( defined[i] && i <= 4095){
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
      addr_1 = (startIndex >> 6) & 0x3F;
      addr_2 = startIndex & 0x3F;

      fprintf(pFile, "%c", addr_1);
      howManyBytes+=2;

      fprintf(pFile, "%c", addr_2);
      howManyBytes+=2;
    }

    val_1 = (memory[i] >> 6) & 0x3F;
    val_2 = memory[i] & 0x3F;

    fprintf(pFile, "%c", val_1);
    howManyBytes+=2;

    fprintf(pFile, "%c", val_2);
    howManyBytes+=2;
    
    i++;
  }

  if(last == TRUE){

    val_1 = (memory[endIndex] >> 6) & 0x3F;
    val_2 = memory[endIndex] & 0x3F;

    fprintf(pFile, "%c", val_1);
    fprintf(pFile, "%c", val_2);

    last = FALSE;
  }

}
