.globl_main
.data
array: .word 100  ,  991  ,  98  ,  97
size : .word 4
.text
main:
	la $s0, array
	lw$s1 , size
	li $t0, 1
	forLoop: slt $t9 , $t0 , $s1
	beq $t9, $zero, exiTforLoop
	sll $t3, $t0, 2
	add $t3, $t3, $s0
	lw $t1, 0($t3)
	add $t2, $t0 , $zero
	whileLoop:	slt $t9, $zero, $t2
			beq $t9, $zero, exiTwhileloop
			addi $t4, $t2, -1
			sll $t3, $t4, 2
			add $t3, $t3, $s0
			lw $t5, 0($t3)
			addi $s3, $s3, 1
			slt $t9, $t1, $t5
			beq $t9, $zero, exiTwhileloop
			sll $t3, $t2, 2
			add $t3, $t3, $s0
			sw $t5, 0($t3)
			add $t2, $t4	, $zero
			j whileLoop	
	exiTwhileloop:  sll $t3, $t2, 2	
			add $t3, $t3, $s0
			sw $t1, 0($t3)
			addi $t0, $t0, 1
			j forLoop
	exiTforLoop:
