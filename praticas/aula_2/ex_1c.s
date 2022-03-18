	# Para o valor da frequência, alterar a constante do while para:
	# 10 Hz -> 2.000.000
	# 5 Hz -> 4.000.000
	# 1 Hz -> 20.000.000					
					# Mapa de registos:
					# counter: $t0
	.equ READ_CORE_TIMER,11
	.equ RESET_CORE_TIMER,13
	.equ PUT_CHAR,3
	.equ PRINT_INT,6		# $a0 = value, $a1 = base
	.text
	.globl main
main:
	li $t0,0			# counter = 0

while1:					# while(1){
	li $v0,RESET_CORE_TIMER
	syscall				# resetCoreTimer();

while2:	li $v0,READ_CORE_TIMER
	syscall				# $v0 = readCoreTimer();
	blt $v0,200000,while2		# while(readCoreTimer() < 200000){
	move $a0,$t0
	li $a1,0x0004000A		# 10 | 4 << 16
	li $v0,PRINT_INT
	syscall				# printInt(counter++, 10 | 4 << 16);

	addi $t0,$t0,1			# counter++;			
endw2:					# }
	li $a0,'\r'
	li $v0,PUT_CHAR	
	syscall				# putChar('\r'); -> cursosr regressa ao inicio da linha

	j while1
endw1:					# }
	jr $ra				# Fim da função main
