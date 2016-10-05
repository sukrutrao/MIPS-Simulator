.globl_main
.data
a1: .word 100
a2: .word 991
a3: .word 98
a4: .word 97
size : .word 4
.text
main:
	addi $sp, $sp, -12
	lw $t0, a1
	sw $t0, 0($sp)
	lw $t0, a2
	sw $t0, 4($sp)
	lw $t0, a3
	sw $t0, 8($sp)
	lw $t0, a4
	sw $t0, 12($sp)
	add $s0, $sp, $zero
	lw $s1 , size
	addi $t0, $zero, 1
	addi $s7, $zero, 4
	forLoop:
	slt $t9 , $t0 , $s1
	beq $t9, $zero, exiTforLoop
	mul $t3, $t0, $s7
	add $t3, $t3, $s0
	lw $t1, 0($t3)
	add $t2, $t0 , $zero
	whileLoop:
			slt $t9, $zero, $t2
			beq $t9, $zero, exiTwhileloop
			addi $t4, $t2, -1
			mul $t3, $t4, $s7
			add $t3, $t3, $s0
			lw $t5, 0($t3)
			addi $s3, $s3, 1
			slt $t9, $t1, $t5
			beq $t9, $zero, exiTwhileloop
			mul $t3, $t2, $s7
			add $t3, $t3, $s0
			sw $t5, 0($t3)
			add $t2, $t4	, $zero
			j whileLoop
	exiTwhileloop:
			mul $t3, $t2, $s7
			add $t3, $t3, $s0
			sw $t1, 0($t3)
			addi $t0, $t0, 1
			j forLoop
	exiTforLoop:
			halt
