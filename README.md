dcc
===

A decaf compiler.

[![Build Status](https://travis-ci.org/ParthKolekar/dcc.svg?branch=master)](https://travis-ci.org/ParthKolekar/dcc)

Decaf Language
--------------

Decaf specifications are as in the given decaf manual. A few notable edits were made to this.

- The for loop does not implicitly contain an increment of the loop index, for flexiblity.
- The loop index termination condition is equated as a not equal condition and not a less than equal condition.
- The loop index needs to be declared.
- The variables are all i64 signed integers so that conditions like `i = j == k` work properly
- Boolean is zero extended to i64 signed integer.
- The special escape characters are not excaped in strings.

Decaf specification changes over the years, the implementation is done strictly for MIT '10 specification which is also 
provided with the source. With the above mentioned changes.

dcc
---

It is a llvm frontend to the decaf language. The output llvm is printed on the stdout,
the errors are in stderr. 
There is no error handling. On detection of fatal errors, the parsing simply stops.
The output llvm can be further processed by `clang`, or `lli`

Build Instructions
------------------

Run `make` on the directory. 
`make debug` runs it in debug mode which prints extra debug information.

Run Instructions
----------------

`dcc filename` prints out the llvm for file on stdout, and errors if any on stderr.
