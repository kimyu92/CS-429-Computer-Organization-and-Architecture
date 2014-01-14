#############################################################
#
#  Makefile for building a PDP-8 assembler
#
CFLAGS=-Wall -O0 -ggdb3


asm8:  objmem.o opcodes.o symtab.o token.o asm8.o 
	gcc ${CFLAGS} $^ -o asm8

asm8.o: asm8.c asm8.h objmem.h opcode.h symbol.h token.h
	gcc ${CFLAGS} asm8.c -c

objmem.o: objmem.c asm8.h
	gcc ${CFLAGS} objmem.c -c

opcodes.o: opcodes.c asm8.h opcode.h
	gcc ${CFLAGS} opcodes.c -c

symtab.o: symtab.c asm8.h objmem.h symbol.h
	gcc ${CFLAGS} symtab.c -c

token.o: token.c asm8.h token.h
	gcc ${CFLAGS} token.c -c

token.h: opcode.h symbol.h
opcode.h: asm8.h


clean:
	rm *.o asm8
