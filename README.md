# README

### Description

This is the final submission for the CS 4386 Project. The given code will compile
code written in the example TL13 language into equivalent C code. It will also catch
errors such as duplicate declaration, no declaration, and mismatched types.

### How to run in UNIX based systems

#### ENSURE YOU HAVE `gcc`, `flex`, and `bison` INSTALLED

Attatched is a script called setUp.sh. Simply run `./setUp.sh` in
your terminal, and the `compiler` executable will be created. Then run
the `compiler` executable on an input of your choice. 1 valid and
a variety of invalid input files have been provided.

ex. `./compiler < correct_input.txt`

Attached is a script called `test.sh` that can be run to immediately test
on all given input files. Make sure that `setUp.sh` has alredy been run

### How to run in Windows based systems

don't
