	
	.equ SFR_BASE_HI, 0xBF88
	.equ PORTB, 0x6050 	# Ler
	.equ TRISB, 0x6040 	# Selecionar Entra/ Saída
	.equ TRISE, 0X6100 	# Selecionar Entrada/ Saída
	.equ LATE, 0X6120 	# Escrever
	
	.data
	.text
	.globl main
main:
	lui $t0, SFR_BASE_HI	# $t0 = 0xBF880000

	# configurar RB0 como entrada
	lw $t1, TRISB($t0)
	ori $t1,$t1,0x0001 	# colocar 1º bit a 1
	lw $t1, TRISB($t0)

	# confgurar RE0 como saída
	lw $t1, TRISE($t0)
	andi $t1,$t1,0xFFFE	# só alterou 1 bit
	sw $t1,TRISE($t0)

loop:	lw $t1,PORTB($t0)	# ler porto
	andi $t1,$t1,0x0001	# Isolar o bit 0

#	xori $t1,$t1,0x0001 -> Ex 2

	# READ - MODIFY - WRITE (Ler sempre o LATE)
	lw $t2, LATE($t0)

	# Juntar a informação o portoB com o Late
	# Mas para garantir que o valor que fica no bit 0 do $t2 é o valor certo
	andi $t2,$t2,0xFFFE	# reset bit 0
	or $t2,$t2,$t1		# merge

	# Escrever o valor no porto de Saída (RE0)
	sw $t2, LATE($t0)
	j loop
endloop:

	jr $ra 			# Fim da função main
