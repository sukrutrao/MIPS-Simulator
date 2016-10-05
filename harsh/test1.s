.globl main
.data 
m: .word -10 , -5 , -9 , -11 , 456 , 986 , -96 , -456 , 78, 45  #array 
size  : .word 9#array size
.text
main:
	la $t0,m #to store the array address
	lw $s1,size#to store the size in s1
	lw $s3,0($t0)#this is my maximum variable , assuming that 1st one is max
	li $t2, 0#the loop variable
	li $t7, 1#to store the array offset to get value
	addi $s1, $s1, -1
	loop:#loop label
		slt $t3, $t2, $s1#comparing if loop should be executed or not
		beq $t3, $zero, exit#checking
		sll $t4, $t7, 2#left shift for array offset
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
