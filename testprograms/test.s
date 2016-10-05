 # sfsdg
.data
n1   : .word 10
m: .word 6
o: .word 7

.text
main   :

a1:
	lw $s0    , n1
	lw $t0, m
	add $s1, $s0, $zero
	addi $s2, $s0, 0
	sub $s3, $s0, $zero
	mul $s4, $s0, $t0
	and $s5, $s4, $s3
	andi $s6, $s5, 1
	or $s7, $s4, $s3
	ori $t8, $s7, 1
	addi $t9, $t8, -1
	nor $t1, $t9, $zero
	slt $t2, $t1, $t0
	slt $t3, $t0, $s0
	slti $t4, $t0, 100
	slti $t5, $t0, 4
	sw $t4, o
	addi $sp, $sp, -4
	sw $t9, 0($sp)
	add $t0, $sp, $zero
	lw $t1, 0($t0)
	sw $t6, 0($t0)
	sw $t4, 0($sp)

	halt
