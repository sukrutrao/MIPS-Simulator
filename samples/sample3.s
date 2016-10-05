# Program to accept N, take its bitwise or with 123 and store it in result
.data
N: .word 1234
result: .word 0

.text
main:
  lw $t0, N
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  ori $t0, $t0, 123
  sw $t0, result
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  halt
