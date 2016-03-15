	.text 
	.align 2 
	.globl main
main:
	subu $sp, $sp, 32 
	sw $ra, 20($sp)
	sd $a0, 32($sp)
	sw $0, 24($sp)
	sw $0, 28($sp)
	lw $t4, addr
	lw $t5, 8($t4)
loop:
	lw $t6, 28($sp)
	mul $t7, $t6, $t6
	lw $t8, 24($sp)
	addu $t9, $t8, $t7
	sw $t9, 24($sp)
	addu $t0, $t6, $t5
	sw $t0, 28($sp)
	ble $t0, 100, loop

	la $a0, str
	li $v0, 4
	syscall

	lw $a0, 24($sp)
	li $v0, 1
	syscall

	la $a0, newline
	li $v0, 4
	syscall

	move $v0, $0
	lw $ra, 20($sp)
	addu $sp, $sp, 32 
	j $ra
	
	.data
	.align  2
str:
	.asciiz "The sum from 0 .. 100 is: "
	.word 	-1
newline:
	.asciiz "\n"
addr:
	.word	inc1
inc1:
	.word	 1
inc2:
	.word	 2
