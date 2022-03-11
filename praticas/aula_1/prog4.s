				# Mapa de Registos:
				# value: $t0	
	.equ READ_INT10,5
	.equ PRINT_INT10,7
	.equ PRINT_INT,6 # $a0 = value, $a1 = base
	.equ PRINT_STRING,8
	.data
str1:	.asciiz "\nIntroduza um inteiro (sinal e módulo): "
str2:	.asciiz "\nValor em base 10 (signed): "
str3: 	.asciiz "\nValor em base 2: "
str4: 	.asciiz "\nValor em base 16: "
str5:	.asciiz "\nValor em base 10 (unsigned): "
str6:	.asciiz "\nValor em base 10 (unsigned), formatado: "
	.text
	.globl main
main:
while:
	la $a0,str1
	li $v0, PRINT_STRING	
	syscall			# printStr(str1)
	li $v0,READ_INT10
	syscall
	move $t0, $v0		# value = readInt10();	
	
	la $a0,str2
	li $v0, PRINT_STRING
	syscall			# printStr(str2)
	move $a0,$t0
	li $v0,PRINT_INT10
	syscall			# PrintInt10(value);

	la $a0,str3
	li $v0,PRINT_STRING	# PrintStr(str3)
	syscall
	move $a0,$t0
	li $a1, 2
	li $v0, PRINT_INT
	syscall			# Print_Int(value, 2);
	
	la $a0,str4
	li $v0,PRINT_STRING
	syscall			# PrintStr(str4)
	move $a0,$t0
	li $a1,16
	li $v0,PRINT_INT
	syscall			# Print_Int(value, 16);

	la $a0,str5
	li $v0,PRINT_STRING
	syscall			# PrintStr(str5)
	move $a0,$t0
	li $a1,10
	li $v0,PRINT_INT
	syscall			# Print_Int(value, 10);

	la $a0,str6
	li $v0,PRINT_STRING
	syscall			# PrintStr(str6)
	move $a0,$t0
	li $a1,0x0005000A
	li $v0,PRINT_INT
	syscall			# printInt(value, 10 | 5 << 16);

	j while			# ciclo infinito -> while(1)				
endw:
	li $v0,0		# return 0;
	jr $ra			# Fim do programa main
