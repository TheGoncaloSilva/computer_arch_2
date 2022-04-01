	.equ TRISB,0x6040
	.equ PORTB,0x6050
	.equ LATB,0x6060

	.equ TRISE,0x6100
	.equ PORTE,0x6110
	.equ LATE,0x6120	

	.equ SFR_BASE_HI,0xBF88
	.equ PRINT_INT10,7
	.equ READ_CORE_TIMER,11
	.equ RESET_CORE_TIMER,12
	.data
	.text
	.globl main
main: 	
	addiu $sp,$sp,-12		# Prólogo
	sw $ra,0($sp)			# Guardar $ra
	sw $s0,4($sp)			# Guardar counter
	sw $s1,8($sp)				

conf:	
	lui $t0, SFR_BASE_HI		# $t0 = 0xBF880000
	
	# configurar RB0-RB3 como entrada
	lw $t1, TRISB($t0)
	ori $t1,$t1,0x000F		# Isolar colocar os 4 bits 1 
	lw $t1, TRISB($t0)	

	# configurar RE0-RB3 como saída
	lw $t1, TRISE($t0)
	andi $t1,$t1,0xFFF0		# Isolar 4 bits 
	sw $t1, TRISE($t0)
	
	li $s0,0x0000			# counter = 0;
loop:
	lui $t0, SFR_BASE_HI		# $t0 = 0xBF880000
		
	# Ler o valor do porto RB3
	lw $s1,PORTB($t0)
	andi $s1,$s1,0x0004		# Isolar o valor de RB2

	# Read - Modify - Write
	lw $t1, LATE($t0)
	andi $t1,$t1,0xFFF0		# Escolher RE0-RE3
	or $t1,$t1,$s0			# merge
	sw $t1, LATE($t0)

	# DEBUG
	move $a0,$s0
	li $v0, PRINT_INT10
	syscall				# printInt10(counter)

	li $a0,667			# 1000ms = 1s = 1Hz
	jal delay

if:	bne $s1,1,else			# if(RB2 == 1)

	andi $t1,$s0,0x0008		# Isolar MSB
	srl $t1,$t1,3

	sll $s0,$s0,1			# counter << 1
	andi $t0,$s0,0x0001		# Isolar o LSB
	xori $t0,$t0,0x0001		# negar o LSB
	xor $t0,$t0,$t1			# xor com o MSB e o LSB

	or $s0,$s0,$t0			# Merge	

	j endif
else:
	andi $t1,$s0,0x0008		# Isolar MSB
	sll $t1,$t1,3

	srl $s0,$s0,1			# counter << 1
	andi $t0,$s0,0x0001		# Isolar o LSB
	xori $t0,$t0,0x0001		# negar o LSB
	xor $t0,$t0,$t1			# xor com o MSB e o LSB

	or $s0,$s0,$t0			# Merge	
endif:	
	
	andi $s0,$s0,0x000F 		# Isolar 4 bits (ou seja, impede a escrita nos registos 
					# acima de RB3 e impede o contador de ultrapassar 0xF)

	j loop
endloop:	
	lw $ra,0($sp)			# Repor $ra
	lw $s0,4($sp)			# Repor counter
	lw $s1,8($sp)
	addiu $sp,$sp,12		# Epílogo
	li $v0,0			# return 0;

	jr $ra				# Fim da função main

######################################## delay #####################################

	# Função terminal que gera um atraso temporal programável de 1 ms
	# void delay(unsigned int ms)

	.text
	.globl delay
delay:	
	li $v0,RESET_CORE_TIMER	
	syscall				# resetCoreTimer();
while2:
	li $v0,READ_CORE_TIMER
	syscall
	mul $t0,$a0,20000
	blt $v0,$t0,while2		# while(readCoreTimer() < k * ms);
	
	jr $ra				# Fim da função delay	

