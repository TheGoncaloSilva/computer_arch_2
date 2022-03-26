	.equ SFR_BASE_HI,0xBF88

	.equ TRISB,0x6040
	.equ PORTB,0x6050
	.equ LATB,0x6060

	.equ TRISE,0x6100
	.equ PORTE,0x6110
	.equ LATE,0x6120

	.equ TRISD,0x60c0
	.equ PORTD,0x60d0
	.equ LATD,0x60e0
 
	.equ READ_CORE_TIMER,11
	.equ RESET_CORE_TIMER,12

	.data
	.text
	.globl main
main:
	addiu $sp,$sp,-8 		# Reservar espaço na stack
	sw $ra,0($sp)			# Guardar $ra
	sw $s0,4($sp)			# Guardar $s1
	
	lui $t0, SFR_BASE_HI 		# $t0 = 0xBF880000

conf:
	# configurar RE0-3 como saída
	lw $t1, TRISE($t0)
	and $t1,$t1,0xFFF0		# alterar o 4 bits
	sw $t1, TRISE($t0)

	# configurar RD0 como saída
	lw $t1, TRISD($t0)
	and $t1,$t1,0xFFFe		# isolar o 1º bit
	sw $t1,TRISD($t0)
	
	li $s0,0			# counter = 0; -> guardar os registos

loop:		
	lui $t0, SFR_BASE_HI 		# $t0 = 0xBF880000
	
	# READ - MODIFY - WRITE (Ler sempre o LATE) -> Ler -> Modificar -> Escrever 
	lw $t2, LATE($t0)
	andi $t2,$t2, 0xFFF0		# reset bits 3-0
	or $t2,$t2,$s0			# merge

	sw $t2, LATE($t0)		# Escrever em RE
	
	# read-modify-write -> Saída no porto RD0
	lw $t2, LATD($t0)
	andi $t2,$t2,0xFFFe		# alterar o 1º bit
	or $t2,$t2,$s0			# merge
	sw $t2, LATD($t0)		# Escrever em RD0

	li $a0,500
	jal delay

	addi $s0,$s0,1			# counter++;
	andi $s0,$s0,0x0000F  		# anotação no tablet


	j loop
endloop:
	lw $ra,0($sp)			# repor $ra
	lw $s0,4($sp)			# repor $s0
	addiu $sp,$sp,8			# libertar a stack
	
	jr $ra				# Fim do programa
	
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


