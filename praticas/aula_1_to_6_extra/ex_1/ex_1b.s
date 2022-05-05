	.equ SFR_BASE_HI, 0xBF88
	.equ PORTB, 0x6050
	.equ TRISB, 0x6040
	.equ TRISE, 0x6100
	.equ LATE, 0x6120
	.data
	.text
	.globl main
main:
	lui $t0,SFR_BASE_HI		# $t0 = 0xBF880000 (carregar os valores nos 16 MSBs)
	
	# Configurar os bits RB3-RB0 como entrada = 1
	lw $t1,TRISB($t0)		# Read
	ori $t1,$t1,0x000F		# Modify (Colocar os 4 LSBs a 1)
	sw $t1,TRISB($t0)		# Write

	# Configurar os bits RE3-RE0 como saída = 0
	lw $t1,TRISE($t0)		# Read
	andi $t1,$t1,0xFFF0		# Modify (colocar os 4 LSBs a 0)
	sw $t1,TRISE($t0)		# Write

loop:
	lui $t0,SFR_BASE_HI		# $t0 = 0xBF880000 (carregar os valores nos 16 MSBs)
	
	# Ler os valores presentes nos portos RB3-RB0
	lw $t1,PORTB($t0)
	andi $t1,$t1,0x000F		# Isolar o valor dos 4 LSBs -> RB3-RB0
	xori $t1,$t1,0x000F		# Inverter a ordem dos bits

	# Mostrar (Escrever) o valor nos 4 LEDS
	lw $t2,LATE($t0)		# Read
	andi $t2,$t2,0xFFF0		# Modify (Isolar o valor dos restantes bits e reiniciar os 4 LSBs)
	or $t1,$t1,$t2			# Merge dos dois valores
	sw $t1,LATE($t0)		# Write

	j loop
endloop:
	jr $ra					# Fim da sub-rotina main
