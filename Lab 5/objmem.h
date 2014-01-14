/* 
   Assembler for PDP-8.  Memory and object file creation header file
*/

/* variables */
Address entry_point;


/* prototypes */
void Clear_Object_Code(void);
void Define_Object_Code(Address addr, INST inst, Boolean redefine);
INST Fetch_Object_Code(Address addr);
void Output_Object_Code(void);

