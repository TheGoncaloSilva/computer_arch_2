	.equ SFR_BASE_HI, 0xBF88
	.equ TRISE, 0x6100
	.equ LATE, 0x6120
	.equ INKEY, 1
	.equ READ_CORE_TIMER,11
	.equ RESET_CORE_TIMER,12
	.data
	.text
	.globl main
main:
	addiu $sp,$sp,-4			# Reservar espaço na stack
	sw $ra,0($sp)			# Salvaguardar $ra

	lui $t0, SFR_BASE_HI	# $t0 = BF880000 -> valor para os 16 MSBs

	# Configurar RE0-RE3 como saída = 0
	lw $t1, TRISE($t0)		# Read
	andi $t1,$t1,0xFFF0		# Modify -> RE0-RE3 como saída
	sw $t1, TRISE($t0)		# Write

loop:
	
	li $v0,INKEY
	syscall

if_0: bne $v0,'0',elif_1
	li $t2,0x0001			# RE0 = 1;
	j endif
elif_1: bne $v0,'1',elif_2
	li $t2,0x0002			# RE1 = 1;
	j endif
elif_2: bne $v0,'2',elif_3
	li $t2,0x0004			# RE2 = 1;
	j endif
elif_3: bne $v0,'3',else
	li $t2,0x0008			# RE3 = 1;
	j endif
else:
	li $t2,0x000F			# RE0-RE3 = 1;
	j endif
endif:

	lui $t0, SFR_BASE_HI	# $t0 = BF880000 -> valor para os 16 MSBs
	# Escrever o valor dos LEDS
	lw $t1, LATE($t0)		# Read
	andi $t1,$t1,0xFFF0		# Modify -> Reiniciar os RE0-RE3
	or $t1,$t1,$t2			# Merge com o novo valor dos LEDS
	sw $t1, LATE($t0)		# Write
	
	li $a0,1000
	jal delay

	lui $t0, SFR_BASE_HI	# $t0 = BF880000 -> valor para os 16 MSBs
	# Reiniciar o valor dos LEDS
	lw $t1, LATE($t0)		# Read
	andi $t1,$t1,0xFFF0		# Modify -> Reiniciar os RE0-RE3
	sw $t1, LATE($t0)		# Write

	j loop
endloop:

	lw $ra,0($sp)			# Repor $ra
	addiu $sp,$sp,4			# Libertar espaço da stack
	jr $ra 					# Fim do programa

############################# Delay ##################################

	# Sub-rotina terminal que gera um atraso temporal programável de 1 ms
	# void delay(unsigned int ms)

	.text
	.globl delay
delay:	

	li $v0,RESET_CORE_TIMER	
	syscall				# resetCoreTimer();
while:
	li $v0,READ_CORE_TIMER
	syscall
	mul $t0,$a0,20000
	blt $v0,$t0,while		# while(readCoreTimer() < k * ms)

	jr $ra					# Fim da sub-rotina delay
