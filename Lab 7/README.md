<h1> Programming Assignment 7 -- CS429 Fall 2013 </h1>
Due Date: 05 December 2013

<h2> Useful GDB commands </h2>

<p>
* x address Prints the value at the address location
* x /20b address Prints the next 20 bytes starting from the address
* x/20c address Prints the next 20 characters starting from the address
* p/x $reg Prints the value stored in the register.
* Example: p/x $eax will print the value of register %eax
* break name Breaks at the beginning of the function name
* break *address Breaks the execution at the address specified
* stepi Executes one instruction
* cont Executes upto the next breakpoint and waits for user response
* run To start the execution
* q To quit debugger
* info Help. Similar to man in Unix.

</p><p>
