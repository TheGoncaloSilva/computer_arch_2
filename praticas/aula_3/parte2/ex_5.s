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
	addiu $sp,$sp,-8		# Prólogo
	sw $ra,0($sp)			# Guardar $ra
	sw $s0,4($sp)			# Guardar counter
	
conf:	
	lui $t0, SFR_BASE_HI		# $t0 = 0xBF880000
	
	# configurar RE0-RE3 como saída
	lw $t1, TRISE($t0)
	andi $t1,$t1,0xFFF0		# Isolar 4 bits 
	sw $t1, TRISE($t0)
	
	li $s0,0x0000			# counter = 0;
loop:
	lui $t0, SFR_BASE_HI		# $t0 = 0xBF880000
		
	# Read - Modify - Write
	lw $t1, LATE($t0)
	andi $t1,$t1,0xFFF0		# Escolher RE0-RE3
	or $t1,$t1,$s0			# merge
	sw $t1, LATE($t0)

	# DEBUG
	move $a0,$s0
	li $v0, PRINT_INT10
	syscall				# printInt10(counter)

	li $a0,667			# 1/x=1,5 <=> x ≃ 0,667
	jal delay

	andi $t1,$s0,0x0008		# Isolar MSB
	srl $t1,$t1,3

	sll $s0,$s0,1			# counter << 1
	andi $t0,$s0,0x0001		# Isolar o LSB
	xori $t0,$t0,0x0001		# negar o LSB
	xor $t0,$t0,$t1			# xor com o MSB e o LSB

	or $s0,$s0,$t0			# Merge			

	andi $s0,$s0,0x000F 		# Isolar 4 bits (ou seja, impede a escrita nos registos 
					# acima de RB3 e impede o contador de ultrapassar 0xF)

	j loop
endloop:	
	lw $ra,0($sp)			# Repor $ra
	lw $s0,4($sp)			# Repor counter
	addiu $sp,$sp,8			# Epílogo
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

