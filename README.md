###This is a compiler and an interpreter for extended PL/0 grammar, which covers most part of Pascal.

The extended PL/0 grammar is similar to Pascal S grammar (more details below).

Some example source codes of the grammar are provided in `test/`.

If you have any question, feel free to reach me at huangrui@buaa.edu.cn. You can find more about me at [my blog](hrlty.github.io).

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

License:

Copyright (C) 2016 by Rui Huang huangrui@buaa.edu.cn

The program CompilerForExPL0 is licensed under the GNU General Public License.

CompilerForExPL0 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

CompilerForExPL0 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with CompilerForExPL0. If not, see http://www.gnu.org/licenses/.





