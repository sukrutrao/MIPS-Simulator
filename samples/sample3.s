# Program to accept N, take its bitwise or with 123 and store it in result
.data
N: .word 1234
result: .word 0

.text
main:
  lw $t0, N # $t0 contains N
  addi $sp, $sp, -4 # move stack pointers backward by 4 bytes
  sw $t0, 0($sp) # store in stack
  ori $t0, $t0, 123 # or immediate with 123
  sw $t0, result  # store result
  lw $t0, 0($sp)  # restore $t0
  addi $sp, $sp, 4  # restore stack pointer
  halt
