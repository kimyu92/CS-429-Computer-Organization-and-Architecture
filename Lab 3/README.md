<h1> Programming Assignment 3 -- CS429 Fall 2013 </h1>
Due Date: 13 October 2013

<h2> Purpose </h2>

<p>
The purpose of this assignment is mainly to write a C program
using pointers.
You will need to write the program (facts.c), 
compile it, and then execute it.  You can execute it with several different
tests of your own, and when you are satisfied, turn the source in to the
TA for grading (use "turnin").

</p><h2> Motivation </h2>

<p>
In this class, we deal with a lot of facts.  Facts about lots of different
types of computers.  It hurts to try to remember all of them.  So let's
write a program to remember them all for us.

</p><p> 
So what is a fact?  It's a piece of information about a particular property of an object.
For example, 

</p><ul>
<li>Fact: The word-size of the CDC6600 is 60 bits.
</li><li>Fact: The PDP-8 has 8 instructions.
</li><li>Fact: The PDP-8 has a 12-bit address.
</li><li>Fact: The x86 architecture has 8 registers.
</li><li>Fact: The x86_64 architecture has 16 registers.
</li><li>Fact: The x86_64 architecture has a 64-bit address.
</li><li>Fact: Lauren's cat's name is Sasha.
</li></ul>

<p> We want a program to remember all these facts, and then help us answer questions,
like

</p><ul>
<li> Question: What is the address for a PDP-8?
</li><li> Question: How many registers does the x86 have?
</li><li> Question: What is Lauren's cat's name?
</li></ul>

<h2> Program Specification </h2>

<p>
Your problem is to write a program to read a set of facts and remember them.
It should then read a set of questions, look up the answers, and print them.  So, you should
write a program "facts.c" which takes two files for input: 
(1) a set of facts and (2) a set of questions.

</p><p>
The facts file will have a sequence of lines.  Each line will start with an "F" (to indicate
that this is a fact), and then three strings:  (1) the name of an object,
(2) a property of that object, and (3) the value of that property for that object.  The syntax
is 
</p><pre>F [object-name]: [property]=[value]
</pre>
The colon (:), equal sign (=) and newline are the delimiters.
Ignore any lines that are blank, or start with "#" instead of "F".

<p>
The questions file will have a sequence of lines.  Each line will start with a "Q" (to indicate
that this is a question), and then two strings: (1) the name of an object, and
(2) a property of that object, separated by a colon.
Again, ignore any lines that are blank, or start with "#" instead of "Q".
The colon (:) and newline are the delimiters.

</p><p>
The output for a question will be one line that looks like a fact, and is printed to standard output.

</p><p> If the second file (the questions file) is not given, read your questions from standard input.

</p><h2> Example </h2>
<p>
Assume your input facts were (computers.fax):
</p><pre>F CDC6600: number_registers=24
F CDC6600: opcode_bits=6
F PDP11: number_registers=8
F PDP11: opcode_bits=4,8,10
F IBM360: number_registers=16
F IBM360: opcode_bits=8
</pre>
and your input questions were (computers.q):
<pre>Q PDP11: number_registers
Q CDC6600: number_registers
Q IBM360: opcode_bits
</pre>
Then running "facts computers.fax computers.q" will produce output that looks like:
<pre>F PDP11: number_registers=8
F CDC6600: number_registers=24
F IBM360: opcode_bits=8
</pre>

<h2> Error Cases </h2>

Consider the following error cases:
<ul>
<li> Failure to provide the facts file -- print an error message and quit.
</li><li> Cannot open file %s -- print the error message with the file name and exit.
</li><li> First character is not an "F" or "Q".
</li><li> Bad syntax for a fact -- missing the colon or the equal sign of a Fact.
</li><li> Same fact with different values -- keep the most recent value for a property.
</li><li> Question asks about an object-name or property that we do not have a fact for -- print "F object-name property=unknown".
</li><li> Providing more (or less) than one or two file names; you 
always need the facts file, but the questions can either be a file or 
stdin.
</li></ul>

You will need to use malloc and free, and pointers, for this assignment.  If you run out of memory,
print "No more memory", and exit.


<h2> Test Cases </h2>

Provided test cases are
<ul>
<li> <a href="http://cs.utexas.edu/users/peterson/prog3/computers.fax"> computers.fax </a> and <a href="http://cs.utexas.edu/users/peterson/prog3/computers.q"> computers.q </a> which produces <a href="http://cs.utexas.edu/users/peterson/prog3/computers.a"> computers.a </a>.
</li></ul>


<h2> Submission </h2>


You need to submit a single C file named facts.c to program3. More specifically, for submission you should use

<pre>turnin --submit zxy program3 facts.c
</pre>

<p>
Your program should compile with "gcc -Wall facts.c" with no errors or warnings.

</p><p>
We may also run your program under "valgrind"; valgrind should find no memory leaks.


</p><h2> Extensions </h2>

<p>
Extensions to consider:
</p><ul>
<p></p><li> Can you merge the Facts and Questions file into one file, allowing new facts to be added 
on the fly?

<p></p></li><li> Can you allow other types of questions, like "What objects have this property?" or "What objects
have this property and value"?

<p></p></li><li> This is effectively a small data-base.  How would you extend it to a full data-base?

<p></p></li><li> This is the beginning of an artificial intelligence program.  What would you need to do
to make it more natural and intelligent?

</li></ul>

<p>
