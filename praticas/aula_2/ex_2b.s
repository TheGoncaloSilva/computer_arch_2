 # Dado que o valor máximo de um registo inteiro é de 2^(32) - 1 = 4294967295.
 # Ou seja, o valor máximo é ( 4294967295 / 20 000 )(ms) porque 20 000 é 1 ms

					# Mapa de registos:
					# counter: $s0
	.equ READ_CORE_TIMER,11
	.equ RESET_CORE_TIMER,12
	.equ PUT_CHAR,3
	.equ PRINT_INT,6		# $a0 = value, $a1 = base
	.text
	.globl main
main:	addiu $sp,$sp,-8		# Reservar espaço na stack
	sw $ra,0($sp)
	sw $s0,4($sp)

	li $s0,0			# counter = 0

while1:					# while(1){

	li $a0,1000
	jal delay			# delay(1ms); 	

	move $a0,$s0
	li $a1,0x0004000A		# 10 | 4 << 16
	li $v0,PRINT_INT
	syscall				# printInt(counter++, 10 | 4 << 16);

	addi $s0,$s0,1			# counter++;			

	li $a0,'\r'
	li $v0,PUT_CHAR	
	syscall				# putChar('\r'); -> cursosr regressa ao inicio da linha

	j while1
endw1:					# }
	lw $ra,0($sp)
	lw $s0,4($sp)
	addiu $sp,$sp,8			# Libertar espaço da stack
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

