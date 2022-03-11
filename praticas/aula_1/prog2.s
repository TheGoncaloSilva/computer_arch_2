				# Mapa de Endereços
				# $t0 = c

	.equ GET_CHAR,2
	.equ PUT_CHAR,3
	.text
	.globl main
main:	
do:				# {
	li $v0, GET_CHAR
	syscall			
	move $t0, $v0		# c = getChar();
	addi $t0,$t0,1		# c = c + 1;
	move $a0,$t0		# $a0 = value;
	li $v0, PUT_CHAR
	syscall			# putChar();
while:	bne $t0,'\n',do		# } while( c != '\n' );
	li $v0,0		# return 0;
	jr $ra 			# Fim da Função Main
