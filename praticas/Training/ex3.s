	.equ SFR_BASE_HI, 0xBF88
	.equ TRISE, 0x6100
	.equ LATE, 0x6120
	.equ RESET_CORE_TIMER, 12
	.equ READ_CORE_TIMER, 11
	.equ PRINT_INT,6
	.equ PUT_CHAR,3
	.text
	.globl main
main:
	addiu $sp,$sp,-8		# Reservar espaço na stack
	sw $ra,0($sp)			# Salvaguardar $ra
	sw $s0,4($sp)			# Salvaguardar $s0

	lui $t0, SFR_BASE_HI 	# $t0 = 0xBF880000

	# Configurar RE4-RE1 como saída = 0
	lw $t1, TRISE($t0)		# Read
	andi $t1,$t1,0xFFE1		# Modify -> RE4-RE1 = 0
	sw $t1, TRISE($t0)		# Write

	li $s0,0x0009			# valor inicial
loop:
	lui $t0, SFR_BASE_HI 	# $t0 = 0xBF880000
	sll $t2,$s0,1			# counter << 1

	# Ligar os LEDs de acordo com o counter
	lw $t1, LATE($t0)		# Read
	andi $t1,$t1,0xFFE1		# Modify -> Isolar o valor dos restantes bits
	or $t1,$t1,$t2			# Modify -> Merge do counter
	sw $t1, LATE($t0)		# Write
	
	move $a0,$s0
	#li $a1,4
	#sll $a1,$a1,16
	#ori $a1,$a1,2
	li $a1,0x00040002
	li $v0,PRINT_INT
	syscall					# printInt(counter, 2 | (4 << 16));

	li $a0,'\n'
	li $v0,PUT_CHAR	
	syscall				# putChar('\n');

	xor $s0,$s0,0x000F		# Inverter os valores do counter (9->6->9->6->...)
	
	li $a0,140				# 7Hz -> 1/7 = 1,42 -> 140
	jal delay

	j loop
endloop:

	lw $ra,0($sp)			# Repor $ra
	lw $s0,4($sp)			# Repor o valor salvaguardado
	addiu $sp,$sp,8			# Libertar o espaço da stack

	jr $ra					# Fim da sub-rotina main

############################# delay ################################

	# Sub-rotina terminal, não é preciso salvaguardar registos
	# Sub-rotina terminal que gera um atraso temporal programável de 1 ms
	# void delay(unsigned int ms)
	
	.text
	.globl delay
delay:
	li $v0,RESET_CORE_TIMER
	syscall					# resetCoreTimer();
while:
	li $v0,READ_CORE_TIMER	# readCoreTimer();
	syscall
	mul $t0,$a0,20000
	blt $v0,$t0,while		# while(readCoreTimer() < K * ms)

	jr $ra					# Fim da sub-rotina delay
