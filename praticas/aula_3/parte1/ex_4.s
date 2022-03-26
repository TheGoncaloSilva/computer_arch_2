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
	lui $t0, SFR_BASE_HI 		# $t0 = 0xBF880000

conf:	# configurar RB0-3 como entrada
	lw $t1,TRISB($t0)
	or $t1,$t1,0x000F		# colocar 1º bit a 1
	lw $t1, TRISB($t0)

	# configurar RE0-3 como saída
	lw $t1, TRISE($t0)
	and $t1,$t1,0xFFF0		# alterar o 1º bit
	sw $t1, TRISE($t0)

loop:	lw $t1,PORTB($t0)
	andi $t1,$t1,0x000F

	xori $t1,$t1,0x0009

	# READ - MODIFY - WRITE (Ler sempre o LATE) -> Ler -> Modificar -> Escrever 
	lw $t2, LATE($t0)

	andi $t2,$t2,0xFFF0		# Reset primeiros 4 bits
	or $t2,$t2,$t1			# merge

	# Escrever o valor no porto de Saída
	sw $t2, LATE($t0)
	j loop
endloop:

	jr $ra				# Fim do programa
