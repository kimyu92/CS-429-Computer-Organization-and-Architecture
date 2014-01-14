/
/  a test of some very strange PC issues
/
/     A test of PC at the last word of a page.
		orig  0x340
onpage,	0x34
		orig  0x37D
pageret, jmp I page
page,	0
		TAD   onpage
		JMP I backpage
backpage,	pageret
		orig  0x3C0
offpage, 0x3C

/ a test of PC at the end of memory
		orig 0xFFD
endret,	JMP I endgame
endgame, 0
		ISZ COUNT       / this is at 0xFFF
		orig 0x0
		IAC
		IAC
		JMP I backend
backend,	endret


		/  do the tests
		orig  0x40
COUNT,	0

topage, page
toend,	endgame
		
start,	CLA
		JMS I topage
		NOP
		CLA
		DCA COUNT
		JMS I toend
		NOP
		CLA CMA
		DCA COUNT
		JMS I toend
		NOP
		HLT

		end start
