				# Mapa de Registos:
				# $t0: c;
	.equ PUT_CHAR,3
	.equ INKEY,1
	.text
	.globl main
main:
do:	li $v0,INKEY		# {
	syscall
	move $t0,$v0		# c = inkey();
if:	beq $t0,0,else		# if( c != 0 )
	move $a0,$t0		
	li $v0,PUT_CHAR		
	syscall			# putChar( c );
	j endif
	
else: 	li $a0,'.'
	li $v0,PUT_CHAR
	syscall			# putChar('.');
endif:

while: 	bne $t0,'\n',do		# } while( c != '\n' );
	
	li $v0,0 		# return 0;
	jr $ra			# Fim do programa main
