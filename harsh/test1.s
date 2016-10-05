.globl main
.data
m1: .word -10
m2: .word -5
m3: .word -9
m4: .word -11
m5: .word 456  #array
size  : .word 5#array size
.text
main:
	addi $sp, $sp, -16
	lw $t0, m1
	sw $t0, 0($sp)
	lw $t0, m2
	sw $t0, 4($sp)
	lw $t0, m3
	sw $t0, 8($sp)
	lw $t0, m4
	sw $t0, 12($sp)
	lw $t0, m5
	sw $t0, 16($sp)
	addi $t8, $zero, 4
	add $t0, $sp, $zero#to store the array address
	lw $s1,size#to store the size in s1
	lw $s3,0($t0)#this is my maximum variable , assuming that 1st one is max
	addi $t2, $zero, 0#the loop variable
	addi $t7, $zero, 1#to store the array offset to get value
	addi $s1, $s1, -1
	loop:#loop label
		slt $t3, $t2, $s1#comparing if loop should be executed or not
		beq $t3, $zero, exit#checking
		mul $t4, $t7, $t8#left shift for array offset
		add $t0, $t0, $t4#updating loop variable
		lw $t5, 0($t0)#storing loop value
		slt $t6, $t5, $s3#comparing for max
		bne $t6,$zero,exitIF#checking
		add $s3, $t5,$zero#updating max
	exitIF:
		addi $t2,$t2, 1#update loop variaable
		j loop		#loop jump
	exit:
halt
