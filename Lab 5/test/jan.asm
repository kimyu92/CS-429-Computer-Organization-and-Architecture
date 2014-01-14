/
/  A program to add a list of numbers
/

/    some pointers
separator1, str1
separator2, str2
separator3, str3
prompt,  prompts


///////////////////////////////////		
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


///////////////////////////////////		
/  print a string.  One character per word.
/	Address of string is in A register
/  string is zero terminated.

P0,     0
prints,  0
		DCA  P0
P1,		TAD I P0
		SZA RSS
		JMP I prints
		IOT 4,0
		ISZ P0
		CLA
		JMP P1


///////////////////////////////////		
/  print a number, in decimal.
/  number is in the A register.
/  Use divide to get remainder by dividing by 10 to get the digits.
/  remainder digits come out low-order first, so save them, and
/  print them in the reverse order.


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

///////////////////////////////////		
/   read a number from input and return the value

readc, 0    / the read character
value, 0    / the accumulated value
rb4,   0    / zero is before any useful input, non-zero after
m10,   -10  / minus 10
m48,   -48  / minus 48 ('0')

readn,  0
		cla
		dca  value       / set value to zero
		dca  rb4
rloop,	IOT  3,0         / read a character
		sma rss			 / check for EOF
		jmp reof
		tad m48			/ add -'0'
		sma rss
		jmp rbad        / bad character
		dca readc		/ save the read value
		tad readc
		tad m10			/ add -10
		sma
		jmp rbad
		/ we have a digit.
		isz rb4	   	    / no longer in the before part
		cla cll
		tad value
		rtl				/ value * 4
		tad value		/ value * 4 + value
		ral				/ (value * 4 + value) * 2 = value * 10
		tad readc
		dca	value
		jmp rloop
/   if we have a bad character (not '0' ... '9') skip it before, or return if after
rbad,	cla
		tad rb4
		sza rss
		jmp rloop      / rb4 = 0; no number yet, skip leading non-numerics
		cla
		tad value      / load value; skip the eof instruction on return
		isz readn
		jmp i readn

/ on eof, if we have a value, return it; if not just return
reof,   cla
		tad rb4
		sza rss
		jmp i readn    / EOF return
		jmp rbad       / otherwise treat as terminator



///////////////////////////////////		
/   Main program.
/
/   1. write a prompt.
/   2. read numbers until EOF (0xFFF)
/   3. add all numbers together
/   4. print numbers and sum
/
		orig 0x80
sum,    0
loopctr, 0
nptr,    0
ninputs, 0
naddr,  addr_of_numbers

main,   cla
		tad		prompt
		jms 	prints             / print the prompt
		cla
		dca		ninputs
		tad		naddr
		dca		nptr
iloop,	JMS		readn              / read a number
		jmp		endinput           / if EOF
		dca	I	nptr               / save the number
		isz		nptr
		isz		ninputs            / increase count of numbers
		jmp		iloop

endinput, cla                      / EOF on input, now add numbers
	    tad		ninputs
		cma iac
		dca		loopctr            / define -n as loop count
		tad		naddr
		dca		nptr
		dca		sum                / sum = 0
oloop,	cla cll
		tad I   nptr               / print number
		jms	    printn
		cla cll                    / sum =
		tad		sum				   /        sum +
		tad I   nptr               /              next number
		isz     nptr
		szl		/ check for overflow
		jmp     overflow
		/ check if negative
		sma rss
		jmp     overflow

		dca     sum
		isz		loopctr
		jmp     oloop1
		jmp     oloop2
oloop1,	cla
		tad     separator1         /  + 
		jms		prints
		jmp		oloop

/    print sum
oloop2, cla
		tad	    separator2         / = 		
		jms		prints
		cla
		tad     sum
		jms     printn
oloop3, cla
	    tad	    separator3         / newline 		
		jms		prints
		hlt

overflow,  cla
		tad     separator2
		jms     prints
		cla
		tad     overmsg
		jms		prints
		jmp		oloop3

overmsg, overs
overs,	'*'
	'*'
	'*'
	' '
	'O'
	'v'
	'e'
	'r'
	'f'
	'l'
	'o'
	'w'
	' '
	'*'
	'*'
	'*'
	0


///////////////////////////////////		
prompts, 'P'
	'l'
	'e'
	'a'
	's'
	'e'
	' '
	't'
	'y'
	'p'
	'e'
	' '
	'n'
	'u'
	'm'
	'b'
	'e'
	'r'
	's'
	' '
	't'
	'o'
	' '
	'a'
	'd'
	'd'
	':'
	' '
	0

str1,	' '
	'+'
	' '
	0


str2,	' '
	'='
	' '
	0

str3,	10
	0

/   numbers continue as long as memory allows
addr_of_numbers,

		end main
