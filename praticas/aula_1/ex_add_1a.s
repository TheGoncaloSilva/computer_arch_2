				# Mapa de registos:
				# state: $s0
				# cnt: $s1
				# c: $s2
	.equ UP,1
	.equ DOWN,0
	.equ PUT_CHAR,3
	.equ PRINT_INT,6
	.equ INKEY,1
	.text
	.globl main
main:
	addiu $sp,$sp,-16	# Reservar espaço na stack
	sw $ra,0($sp)		# Salvaguardar $ra
	sw $s0,4($sp)		# Salvaguardar $s0
	sw $s1,8($sp)		# Salvaguardar $s1
	sw $s2,12($sp)		# Salvaguardar $s2

	li $s0,0		# state = 0;
	li $s1,0		# cnt = 0;

do:				# do {
	li $a0,'\r'
	li $v0,PUT_CHAR
	syscall			# putChar('\r'); -> caracter de retorno
	move $a0,$s1
	li $a1,0x000300A	# 10 | 3 << 16
	syscall			# printInt( cnt, 10 | 3 << 16); -> decimal c/ 3 digitos
	
	li $a0,'\t'		
	li $v0,PUT_CHAR
	syscall			# putChar('\t'); -> caracter de Tab
	move $a0,$s1
	li $a1,0x0080002	# 2 / 8 << 16
	syscall			# printInt( cnt, 2 | 8 << 16 ); -> binary c/ 8 digitos
	
	li $a0,5
	jal wait		# wait(5); -> Esperar 0.5s
	
	li $v0,INKEY
	syscall			
	move $s2,$v0		# c = inkey();

if_1:	bne $s2,'+',eif_1	# if ( c == '+' )
	li $s0,UP		# state = UP;
eif_1:	
if_2:	bne $s2,'-',eif_2	# if ( c == '-' )
	li $s0,DOWN		# state = DOWN;
eif_2:	
	
if3:	bne $s0,UP,else_3	# if ( state == UP )
	addi $t0,$s1,1		# $t0 = (cnt + 1);	
	andi $s1,$t0,0xFF	# cnt = (cnt + 1) & 0xFF; -> Up counter MOD 256
	j eif_3	
else_3: 
	addi $t0,$s1,-1		# $t0 = (cnt - 1);	
	andi $s1,$t0,0xFF	# cnt = (cnt - 1) & 0xFF; -> Down counter MOD 256
eif_3:
while:	bne $s2,'q',do		# } while( c != 'q');	
	
	addiu $sp,$sp,16	# Reservar espaço na stack
	lw $ra,0($sp)		# Salvaguardar $ra
	lw $s0,4($sp)		# Salvaguardar $s0
	lw $s1,8($sp)		# Salvaguardar $s1
	lw $s2,12($sp)		# Salvaguardar $s2
	
	li $v0,0		# return 0;

	jr $ra			# Fim do Programa main

############################## COMEÇO DA FUNÇÃO WAIT ########################################
				
				# Mapa de registos:
				# ts: $a0
 # Função terminal, não necessita de salvaguardar registos ou usar os do tipo reservado
 # Função que espera uma quantidade de tempo, com o fator TS fornecido em $a0 
	.text
	.globl wait
wait:	
	li $t0,0		# i = 0;
	li $t1,51500	
	mult $t1,$a0		# 515000 * ts (Em instrução nativa)
	mflo $t1		# $t1 = 51500 * ts
for:	bge $t0,$t1,endf	# for( i = 0; i < 515000 * ts; i++) { -> wait ts/10 seconds
	addi $t0,$t0,1		# i++;
	j for			# }
endf:
	jr $ra

	



