#program to count number of 1s in binary representation of a 64-bit integer

#data section
.data
N1: .word 123			#two 32-bit integers, an array N for making it 64 bit
N2: .word 100
result: .word 0     #result for number of 1s

.globl main

#text section

.text

main: 						#s0 contains address of current element of N, here the first element
	  addi $s1, $zero, 0     #s1 contains the result, here 0
	  lw $t0, N1        #t0 contains the current element of N, now the first element
	  add $a0, $t0, $zero   #a0 is the function argument, now the first element of N
	  j Bits1
	  Back1:				#call function with argument a0
	  add $s1, $s1, $v0     #v0 is the function return value, add it to result
	  lw $t0, N2        #load second element of N into t0, so offset is 4
	  add $a0, $t0, $zero   #copy t0 to a0 to pass to function
	  j Bits2              #call function with argument a0
	  Back2:
	  add $s1, $s1, $v0     #add return value to the result

	  sw $s1, result		#store result in memory

	  halt

#function for counting number of 1s
Bits1:
addi $t7, $t7, 2
addi $t0, $zero, 0 	#t0 is i, initialized to 0
	  add $t2, $a0, $zero   #t2 contains modified a0, for now equals a0
	  addi $v0, $zero, 0    #v0 is result, for now intiialized to 0
Loop:
andi $t1, $t2, 65535		#if bitwise and of t2 and 1 is 0
	  beq $t1, $zero, Inc   #if true, go for increment
	  addi $v0, $v0, 1		#if not, add to count of 1s
Inc:
mul $t2, $t2, $t7	#right shift t2 by 1 position
	  addi $t0, $t0, 1		#increment i
	  slti $t3, $t0, 32		#if i<32
	  bne $t3, $zero, Loop	#if not, go back to Loop
	  j Back1				#return back to main

Bits2:
addi $t7, $t7, 2
addi $t0, $zero, 0 	#t0 is i, initialized to 0
	  add $t2, $a0, $zero   #t2 contains modified a0, for now equals a0
	  addi $v0, $zero, 0    #v0 is result, for now intiialized to 0
Loop2:
andi $t1, $t2, 65535		#if bitwise and of t2 and 1 is 0
	  beq $t1, $zero, Inc2   #if true, go for increment
	  addi $v0, $v0, 1		#if not, add to count of 1s
Inc2:
mul $t2, $t2, $t7		#right shift t2 by 1 position
	  addi $t0, $t0, 1		#increment i
	  slti $t3, $t0, 32		#if i<32
	  bne $t3, $zero, Loop2	#if not, go back to Loop
	  j Back2				#return back to main
