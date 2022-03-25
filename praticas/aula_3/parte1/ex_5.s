	.equ SFR_BASE_HI,0xBF88
	.equ TRISB,0x6040
	.equ PORTB,0x6050
	.equ LATB,0x6060
	.equ TRISE,0x6100
	.equ PORTE,0x6110
	.equ LATE,0x6120

	.data
	.text
	.globl main
main:
	addiu $sp,$sp,-8 		# Reservar espaço na stack
	sw $ra,0($sp)			# Guardar $ra
	sw $s1,4($sp)			# Guardar $s1
	
	lui $t0, SFR_BASE_HI 		# $t0 = 0xBF880000

conf:
	# configurar RE0-3 como saída
	lw $t1, TRISE($t0)
	and $t1,$t1,0xFFF0		# alterar o 1º bit
	sw $t1, TRISE($t0)
	
	li $s1,0			# counter = 0; -> guardar os registos
loop:	lw $t2, LATE($t0)
	andi $t2,$t2, 0xFFF0		# reset bits 3-0
	or $t2,$t2,$s1			# merge
	sw $t2, LATE($t0)

	li $a0,500
	jal delay

	addi $s1,$s1,1			# counter++;
	andi $s1,$s1,0x0000F  		# anotação no tablet


	j loop
endloop:
	lw $ra,0($sp)			# repor $ra
	lw $s0,4($sp)			# repor $s0
	addiu $sp,$sp,8			# libertar a stack
	
	jr $ra				# Fim do programa
