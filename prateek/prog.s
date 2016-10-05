.data # data to be stored in memory
SIZE: .word 3
N1: .word 2
N2: .word 5
N3: .word 1


.text
main:
addi $sp, $sp, -8
lw $t0, N1
sw $t0, 0($sp)
lw $t0, N2
sw $t0, 4($sp)
lw $t0, N3
sw $t0, 8($sp)
add $t1, $sp, $zero # address of N
lw $t0, SIZE # store value of SIZE
lw $s0, 0($sp) # store maximum value in N
loop: # Loop that iterates for SIZE times
	lw $t2, 0($t1) # load value at $t1 address in $t2
	addi $t1, $t1, 4 # increase address by 4
	slt $t3, $s0, $t2 # check if max < current number
	bne $t3, $zero, save # if max < current the save in $s0
	j exit
save:
	add $s0, $t2, $zero # save in $s0
exit:
	addi $t0, $t0, -1 # decrease number of elements by 1
	bne $t0, $zero, loop # loop if number of elements != 0
halt
