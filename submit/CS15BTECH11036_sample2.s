# Program to find the nth Fibonacci number
.data
N: .word 20 # value of N
ceilN2: .word 9  # make this equal to (N-1)/2
A: .word 0  # first Fibonacci number
B: .word 1 # second Fibonacci number
result: .word 0 # result

.text
main:
  lw $s0, N # $s0 contains N
  lw $s1, ceilN2 # $s1 contains (N-1)/2
  lw $t0, A # $t0 and $t1 store intermediate results
  lw $t1, B
  slti $t3, $s0, 1 # if N<1
  bne $t3, $zero, Exit # if so, Exit
  addi $t2, $zero, 0 # $t2 is i, i=0
  slt $t3, $t2, $s1 # if i<(N-1)/2
  beq $t3, $zero, Outside # if so, don't enter the loop
  Loop:
    add $t0, $t0, $t1  #update $t0 and $t1
    add $t1, $t0, $t1
    addi $t2, $t2, 1 # i++
    slt $t3, $t2, $s1 # if i<(N-1)/2
    bne $t3, $zero, Loop # if so, continue the loop
  Outside:
    andi $t3, $s0, 1 # and of N with 1, 1 for odd N, 0 for even N
    beq $t3, $zero, Even # if N is even, jump to Even
    Odd:
      sw $t0, result # for odd N, $t0 has the result
      j Exit # Exit
    Even:
      sw $t1, result # for even N, $t1 has the result
  Exit:
    halt
