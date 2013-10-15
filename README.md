sse2
====

Some examples of unusual SSE2 instruction uses, as discussed in my blog mischasan.wordpress.com

Mischa Sandberg  mischasan@gmail.com

These functions are almost always faster than their libc counterparts.
As of writing, glibc has switched to using SSE2 for "strlen" and "strchr".
They are hardware-dependent on SSE2, which both Intel and AMD have supported for a decade.
Surprisingly, the string-related functions are faster than SSE 4.2 "string" operations.
Go figure.

DOCUMENTATION
-------------

Check my blog. These are intended as source examples; if the comments don't cut it for you, let me know.

ssebmx     - bit-matrix transpose, where [rows,cols] are both multiples of 8.
             
ssebndm    - BNDM equivalent to "strstr", that really rocks for patterns 65-128 bytes long, using SSE2 bit ops.

ssecmp     - SSE2 equivalent to "strcmp". This shows how to handle boundary conditions when XMM inputs
             have different (odd) alignments across 16-byte boundaries.
          
ssestr     - SSE2 equivalent to "strstr", that searches for 2-byte tokens. Not rocket science,
             but shows off how you efficiently process null-terminated strings --- most high-speed string
             searches, such as BM or Horspool, need to know the target string length in advance,
             and are poorly suited to "C" strings.

ssesort16d - SSE2 sort of an array of 16 doubles. If you think that's really specialized, you're right.
             It's 10x faster than qsort on my hardware (ymmv).

sserank16d - Similar to ssesort16d, except that it produces a rank index: a permutation of [0..15].
             I find this makes ssesort16d actually useful, when the sort key is a double but there's more
             "value" to what you're sorting.

LICENSE
-------

Though I've had strong suggestions to go with BSD license, I'm going with GPL2 until I figure out
how to keep in touch with people who download and use the code. Hence the "CONTACT ME IF..." line in the license.

GETTING STARTED
---------------

Download the source, type "gmake".

The utility "deas" is a pretty-printer that stitches "C" source files together with "gcc -S" output 
(assembler source code). To see what code gcc generates for, say, ssesort.o, run "gmake ssesort.s"
