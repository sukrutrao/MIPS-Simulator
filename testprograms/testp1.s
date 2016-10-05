.data
N: .word 10


.text
main:
	addi $t0, $zero, -2147483648
	addi $t1, $zero, 1
	mul $t2, $t0, $t1
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	sw $t2, -4($sp)
	lw $t7, -4($sp)
	halt
