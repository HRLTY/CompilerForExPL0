###This is a compiler and an interpreter for extended PL/0 grammar, which covers most part of Pascal.

The extended PL/0 grammar is similar to Pascal S grammar (more details below).

Installation:

`mkdir build`

`cd build`

`cmake .. `

`make`

Note: Please make sure you have a c++11 supported compiler.

Usage:

`./pl0`

Then input the file containing the source code, ex. `../test/test2`. 

After compilation, the object code (P-code) will be generated and saved in file `code`, and the interpreter will begin to interpreter the object code and output results.

A detailed design document with various specifications is provided (in Chinese :D)

####More about Pascal S Grammar

To quickly describe the language, we can compare it to the full-blown WIRTH P4 Pascal. The Pascal S does not contain:

- enumeration
- SET
- files
- pointers
- variant records
- procedural types or conformant arrays
- GOTO
- WITH

The complete BNF of Pascal-S can be found in [here](http://www.delphifeeds.com/go/s/10435).



