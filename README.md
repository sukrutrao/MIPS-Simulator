# MIPS Simulator

[![Build Status](https://travis-ci.com/sukrutrao/MIPS-Simulator.svg?branch=master)](https://travis-ci.com/sukrutrao/MIPS-Simulator)

This program is a simulator to simulate execution of MIPS programs. There are two execution modes supported:
	
* Step by Step Mode - in this mode, the program will run one instruction at a time, and
display the state of the registers and memory after each instruction. The next instruction
will be executed when the user presses a key.
* Execution Mode - The program will execute all the instructions till a halt is encountered.

The instructions supported are add, addi, sub, mul, and, andi, or, ori, nor, slt, slti, beq, bne, lw,
sw, j, and halt. halt is a new instruction, which when encountered causes the program to
terminate.

## Setup and Usage
### Prerequisites
To build the program, a C++ compiler (such as g++) with C++11 support is required.

### Building the program
To compile the code, use the following command:
```bash
$ g++ simulator.cpp -o simulator --std=c++11
```

### Running the simulator
To run the simulator, use the following command:
```bash
$ ./simulator
```

## Guidelines
* The program can contain .data and .text sections. There should be no text, apart from comments or blank lines, between the two sections
* Comments are supported
* The .data section can contain labels pertaining to a single word only. .space declarations are not allowed. Only integer data is supported.
* The .text section must contain a main label
* The entire program can be at most 10000 lines long. The program counter is given by 4 times the line number.
* The two ways of accessing memory are:
	- Declaring labels
	- Using the stack
* Addresses 40000 to 40396 represent the locations of memory elements for a 100 element stack, with each element of size 4 bytes. They can be accessed using the $sp register, which initially points to the last element of the stack.
* Any memory element created in the data section is assigned an address starting from 40400.
* Every program must contain a halt statement and the program ends with the halt
statement
* A line containing a label may not contain any other instruction.
* The stack pointer, or any other register, can be used only to access elements having an
address that is a multiple of 4.
* The registers $zero and $at may not be modified. Any other register may be modified.
$at may not be used in any instruction.
* Any value used must lie between -2147483648 and 2147483647, both inclusive.
* The stack pointer can be moved only in multiples of 4.
* Any label must start with an alphabet, and can contain only numbers and alphabets
* Anything after a halt is not executed, unless execution moves there through jumps. Any
errors in the flow of control after halt are ignored, except if there are any preprocessing
errors, such as redeclaration of data or text sections or labels, or invalid labels.
* Overflows in arithmetic will not throw an error.
* Except in displaying the address, all values used use the decimal number system.

## License
This program is provided under the [MIT License](LICENSE).