This is the fourth project for the CSI402 class at Ualbany. Test cases are provided courtesy of professor Ravi. This program is a compiler for the assembly language known as TMIPS, which is a version of MIPS that is stored in a smaller memory size. This is a 2 pass assembler and will catch errors of the following type:
1) Undefined symbols
2) Multiply defined symbols
3) Invalid Opcode
If the given asm file is devoid of any of those errors then an obj file will be created with the same name. This file will contains the assembled form of the assembly file in HEX. A symbol table file with the .sym extension will also be created with shows all of the symbols present in the assembly file along with their location counter. If any of the errors are found then an err file will be created wth the same file name. This will contain the assembly file along with line numbers as well as the list of errors at the bottom, along with line numbers. 

This program also contains a text file with all of the possible opcodes and additional data, and this is necessary for the program to function. 

The program is run with the .asm assembly file as part of the command line. 

