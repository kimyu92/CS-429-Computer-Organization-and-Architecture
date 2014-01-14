/
/  A program to print all the prime numbers
/
n,		0		/ the number we are working on
ascii_nl, 10
limit,  2000

start,	cla iac	/ 1
		dca n	/ n = 1 (initially)

label1, cla
		tad n
		jms prime		/ check if n is prime, skip if so
		jmp	label2
		cla
		tad n           / n is prime
		jms printn		/ print n
		cla
		tad ascii_nl
		iot 4 0
label2, isz n
		cla 
		tad n
		cma iac         / -n
		tad limit       / limit - n
		sma             / if limit < n, halt
		jmp label1      / loop until all numbers handled
		hlt				/ then stop

/  printn contents of A register (as a decimal number) on device 4

parray, 0
		0
		0
		0
		0        / array of 5 digits.
arradd, parray   / address of print digit array
arrptr, parray   / pointer into array for next digit
tempn,  0        / space to save working value of n
ascii_zero,  '0'
ten,    10

printn,  0
		dca tempn		/ save n
		tad ten
		dca X           / to divide by 10
		tad tempn		/ reload n
plo,	jms div 		/ divide A by 10; quotient in A; remainder in R
		dca tempn       / n = n / 10
        tad r			/ load remainder
		dca i arrptr	/ save digit
		isz arrptr
		tad tempn
		sza				/ are we done? is tempn = 0
		jmp plo
		/ we have broken n into its digits; now print them
		/ load pointer and decrement it
ploo,	cla cma			/ -1 -> A
		tad arrptr
		dca arrptr
		tad i arrptr    / get the digit
		tad ascii_zero  / change to ascii
		iot 4 0			/ output to stdout
		cla
		tad arrptr
		cma iac			/ -arrptr
		tad arradd		/ compare with base address
		sza
		jmp ploo
		jmp i printn     / we done.

/ divide
/ divide A register by the value in X

q,      0    / quotient
r,      0    / remainder

X,      0
div,    0
		dca  r
		dca  q    / set q = 0
		tad  X
		sza rss cla / make sure X is not zero
		jmp I div   / divide by zero, return zero

dloop,  tad  X    /  divide by X
		cma iac   / -X
		tad  r    /  r - X
		spa       / if r > X skip
		jmp  dfini
		isz  q    / q = q + 1 (for the X we just subtracted)
		dca  r    / r = r - X
        jmp  dloop
dfini,  cla
		tad q			/ put quotient in a
		jmp i div     / return 




/ determine if the value in the A register is prime or not
/ if prime, return to RA+1; else return to RA.

prime,  0

/ to determine if the number is prime, divide by every number from 2 .. RA-1
   	    DCA  pcan       / save the candidate value
		IAC RAL         / start with 2
		DCA pnext

ploop,	CLA
		TAD pnext
		CMA IAC         / -pnext
		TAD pcan        / compare pnext and pcan
		SZA RSS
		JMP pis         / pnext = pcan, is prime
		SMA RSS         / pcan - pnext > 0
		JMP pis         / is prime; pcan <= pnext

    	CLA
		TAD pnext
		DCA X
		TAD pcan
		JMS DIV         / See if PCAN / PNEXT is zero.
		CLA
		TAD R          / Look at the remainder
		SZA RSS
		JMP I prime   / if remainder is zero, not prime
		
		ISZ pnext
		JMP ploop

pis,    isz prime       / if number is prime, increment RA
		jmp i prime      

pcan,   0
pnext,  0

		end start
