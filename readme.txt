1. The program can contain .data and .text sections. There should be no text, apart from comments or blank lines, between the two sections
2. Comments are supported
3. The .data section can contain labels pertaining to a single word only. .space declarations are not allowed. Only integer data is supported.
4. The .text section must contain a main label
5. The entire program can be at most 10000 lines long. Addressing starts from 0 at the first line of the program, irrespective of the section. Thus, the .text section may start at different addresses for different programs.
6. Addresses 40000 to 40396 represent the locations of memory elements for a 100 element stack, with each element of size 4 bytes. They can be accessed using the $sp register, which initially points to the last element of the stack.
7. Any memory element created in the data section is assigned an address starting from 40400.
8. Every program must contain a halt statement
9. A line containing a label may not contain any other instruction.
10. The stack pointer, or any other register, can be used only to access elements having an address that is a multiple of 4.
