	.text 
	.align 2 
	.globl main 
	.globl printf
main:
	subu $sp, $sp, 32 
	sw $ra, 20($sp)
	sd $a0, 32($sp)
	sw $0, 24($sp)
	sw $0, 28($sp)
loop:
	lw $t6, 28($sp)
	mul $t7, $t6, $t6
	lw $t8, 24($sp)
	addu $t9, $t8, $t7
	sw $t9, 24($sp)
	addu $t0, $t6, 1
	sw $t0, 28($sp)
	ble $t0, 100, loop

	la $a0, str
	lw $a1, 24($sp)
	jal printf

	move $v0, $0
	lw $ra, 20($sp) 
	addu $sp, $sp, 32 
	j $ra
	
	.data
	.align 0 
str:
	.asciiz "The sum from 0 .. 100 is: "

