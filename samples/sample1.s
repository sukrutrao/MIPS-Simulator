# Program to find the sum, difference, product, bitwise and, bitwise or, and bitwise nor of two numbers
.data
A: .word 1 # A is the first input
B: .word 2 # B is the second input
sum: .word 0
differ: .word 0
product: .word 0
band: .word 0
bor: .word 0
bnor: .word 0

.text
main:
  lw $s0, A # $s0 contains A
  lw $s1, B # $s1 contains B
  add $t0, $s0, $s1 # $t0 contains each result
  sw $t0, sum # store each result
  sub $t0, $s0, $s1
  sw $t0, differ
  mul $t0, $s0, $s1
  sw $t0, product
  and $t0, $s0, $s1
  sw $t0, band
  or $t0, $s0, $s1
  sw $t0, bor
  nor $t0, $s0, $s1
  sw $t0, bnor
  halt
