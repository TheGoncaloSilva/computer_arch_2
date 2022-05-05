	.equ SFR_BASE_HI, 0xBF88
	.equ PORTB, 0x6050
	.equ TRISB, 0x6040
	.equ TRISE, 0x6100
	.equ LATE, 0x6120
	.data
	.text
	.globl main
main:
	lui $t0, SFR_BASE_HI	# $t0 = 0xBF880000 -> Colocar o valor nos 16 MSBs

	# Configurar os portos RB3-RB0 como entrada = 1
	lw $t1, TRISB($t0)		# Read
	ori $t1,$t1,0x000F		# Modify -> RB3-RB0 como entrada
	sw $t1, TRISB($t0)		# Write

	# Configurar os portos RE0-RE7 como saída = 0
	lw $t1, TRISE($t0)		# Read
	andi $t1,$t1,0xFF00		# Modify -> RE0-RE7 como saída
	sw $t1, TRISE($t0)		# Write

loop:
	lui $t0, SFR_BASE_HI	# $t0 = 0xBF880000 -> Colocar o valor nos 16 MSBs
	
	# Ler o valor do dip-switch
	lw $t1,PORTB($t0)
	andi $t1,$t1,0x000F		# Filtrar pelos SWs RB3-RB0
	xor $t2,$t1,0x000F		# Aplicar um filtro para inverter os valores

	# Escrever o valor lido do dip-switch nos LEDS RE3-RE7
	lw $t3, LATE($t0)		# Read
	sll $t1,$t1,4			# Levar o valor para os bits 3-7 -> Falta inverter a ordem
	andi $t3,$t3,0xFF00		# Modify (Isolar o valor dos restantes bits)
	or $t3,$t3,$t1			# Merge com o valor atual
	or $t3,$t3,$t2			# Merge com complemento para 1
	sw $t3, LATE($t0)		# Write

	j loop
endloop:
	jr $ra					# Fim da sub-rotina main
