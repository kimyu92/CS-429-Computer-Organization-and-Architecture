<h1> Programming Assignment 6 -- CS429 Fall 2013 </h1>
Due Date: 24 November 2013

<h2> Purpose </h2>

<p>
The purpose of this assignment is to give you experience with
caches and evaluation of them.  Also, I want to know if this
victim cache idea helps cache behavior.

</p><p>
In addition, a major part of C programming, in the real world, is being able to
take someone else's code and fix it or extend it.  Also, programs
always have to run faster.


</p><h2> Motivation </h2>

<p>
At <a href="http://www.cs.utexas.edu/users/peterson/src/cache.tar"> cache.tar </a>
is a tar file of a C program and its Makefile 
that is a general cache simulator.  This cache simulator takes a 
description of a set of caches (in the description file) and a 
trace file (of memory references), and computes the number of hits
and misses for the described caches for the given memory trace.
This allows us to easily compare different cache designs.<P>

The current cache simulator has the ability to define a "property" to
have a "value", by saying "property=value". For example,
<pre>
{ name=lru, line_size = 128, entries = 1024, ways=4, policy=lru }
</pre>

</p><p> 
Caches are described by their line size and number of entries. All
caches are set-associative.  We can model a direct-mapped cache by 
setting the number of ways to 1; we can model a fully associative
cache by setting the number of ways equal to the number of entries.
In addition, we can specify a replacement policy (LRU, FIFO, LFU, or RANDOM).
If we have a LFU (least frequently used) policy, we need to define a decay interval.

</p><p>
After the cache is described, the memory trace is simulated, and statistics
are printed for each cache design.  For example:

</p><pre>lru: 256 entries of lines of 64 bytes; 64 sets of 4 ways, write-back, LRU
      200000 addresses (133899 Fetch, 39430 Load, 26671 Store)
      205335 hits (99%), 80 misses, 80 memory reads, 0 memory writes
      15 dirty cache lines remain
</pre>


<h2> Program Specification </h2>

Download the cache simulator and modify the code so that it can also
simulate a victim cache.  And we want the program to run no slower
with this new function than it ran before.

<!-- <p>
We can currently specify that a cache is write back
by saying "writeback = true" or "writeback = false", or by saying
"writethru = true" or "writethru = false".   -->

</p><p> To specify that a victim cache is to be simulated, add another
option to the description of a cache.  Add an ability to say "victim =
n" to a cache description, to define a victim cache of n cache lines.

</p><p>
If a cache is defined to include a victim cache, create an
n-entry fully-associative victim cache.  Any cache line that is
evicted from the cache is put in the victim cache.  A memory
reference that is not in the cache causes a search of the victim
cache.<P>

If the memory reference is in the victim cache it is used to
satisfy the request for the cache line.  To satisfy the request,
a victim must be found in the base cache, and then evicted from
the base cache (into the victim cache) while the desired entry
from the victim cache is moved back to the base cache. (The desired cache line, in the
victim cache, is swapped with another line, in the base cache.)
<P>

If the memory reference is not in either the base cache or the victim
cache, we get it from memory and put it in the cache.

</p><h2> Example </h2>

<p>
Suppose we have a cache description (lru_definition):
</p><pre>{ name=lru, line_size = 128, entries = 1024, ways=4, policy=lru }
</pre>
or
<pre>{ name=lru, line_size = 128, entries = 1024, ways=4, policy=lru, victim=0 }
</pre>

For memory trace file test_trace0, we get the following:
<pre> &gt; cachesim lru_definition test_trace0
lru: 1024 entries of lines of 128 bytes; 256 sets of 4 ways, write-back, LRU
      200119 addresses (149800 Fetch, 38425 Load, 11894 Store)
      202606 hits (99%), 1638 misses, 1638 memory reads, 111 memory writes
      99 dirty cache lines remain
</pre>

<p>
Adding a victim cache is just changing the definition to 
</p><pre>{ name=lru, line_size = 128, entries = 1024, ways=4, policy=lru, victim=4 }
</pre>
and now we get
<pre> &gt; cachesim lru_definition test_trace0
lru: 1024 entries of lines of 128 bytes; 256 sets of 4 ways, write-back, LRU
      200119 addresses (149800 Fetch, 38425 Load, 11894 Store)
      202606 hits (99%), 1638 misses, 1638 memory reads, 109 memory writes
      99 dirty cache lines remain
      4 victim cache hits
</pre>


<h2> Test Cases </h2>
<p>
The cache simulator takes valgrind memory traces as input.  You can produce your
own trace files by:
</p><pre>valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes {command to trace} ...
</pre>
for any command that you want to trace.  There are sample trace files in
<a href="http://www.cs.utexas.edu/users/peterson/memory_trace"> peterson/memory_trace </a>.
Be careful, these can get large quickly.  Our samples are between 3 and 6 Megabytes.

<h2> Submission </h2>

You need to submit files to create an executable called cachesim for program6.

More specifically, for submission you should use

<pre>
turnin --submit yuhang program6 Makefile *.h *.c
</pre>

<p>
