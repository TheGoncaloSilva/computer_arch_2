	# Sub-rotina main é uma função intermédia, ou seja, temos de salvaguardar
 # o registo $ra e outros que necessitarmos, nos registos $sX. 				
				# Mapa de registos
				# 
	.equ SIZE,20
	.equ PRINT_STRING,8
	.equ READ_STRING,9	# $a0 = buffer, $a1 = nº caracteres
	.equ PRINT_INT,6	# $a0 = val, $a1 = base
	.equ PRINT_INT10,7
	.data
txt1:	.asciiz "Introduza 2 strings: "
txt2:	.asciiz "Resultados:\n"
	.align 2
str1: 	.space 21		# static char str1[SIZE + 1];
	.align 2
str2: 	.space 21		# static char str2[SIZE + 1];
	.align 2
str3: 	.space 41		# static char str3[2 * SIZE + 1];
	.text
	.globl main
main:	
	addiu $sp,$sp,-4	# guardar espaço na stack
	sw $ra,0($sp)		# Salvaguardar $ra	

	la $a0,txt1
	li $v0,PRINT_STRING
	syscall			# printStr("Introduza 2 strings: ");
	la $a0,str1		# $a0 = buffer
	li $a1,SIZE		# $a1 = numero de caracteres
	li $v0,READ_STRING
	syscall			# readStr( str1, SIZE );
	la $a0,str2		# $a0 = buffer
	li $a1,SIZE		# $a1 = numero de caracteres
	li $v0,READ_STRING
	syscall			# readStr( str2, SIZE );
	la $a0,txt2
	li $v0,PRINT_STRING
	syscall			# printStr("Resultados:\n");
	
	la $a0,str1
	jal strlen		# $v0 = strlen(str1);
	move $a0,$v0
	li $a1,10
	li $v0,PRINT_INT		
	syscall			# printInt( strlen(str1), 10 );

	la $a0,str2
	jal strlen		# $v0 = strlen(str2);
	move $a0,$v0
	li $a1,10
	li $v0,PRINT_INT		
	syscall			# printInt( strlen(str2), 10 );

	la $a0,str3
	la $a1,str1
	jal strcpy		# strcpy(str3, str1);

	la $a0,str3
	la $a1,str2
	jal strcat		# $v0 = strcat(str3, str2);
	move $a0,$v0
	li $v0,PRINT_STRING
	syscall			# printStr( strcat(str3, str2) );
	
	la $a0,str1
	la $a1,str2
	jal strcmp		# $v0 = strcmp(str1, str2);
	move $a0,$v0
	li $v0,PRINT_INT10
	syscall			# printInt10( strcmp(str1, str2) );

	lw $ra,0($sp)		# repor $ra
	addiu $sp,$sp,4		# Libertar o espaço da stack

	li $v0,0		# return 0;
	jr $ra 			# Fim do programa main

################################ Strlen #####################################

	# Sub-rotina terminal que devolve o tamanho da string "str" (excluindo o caracter null)
	# int strlen(char *str) 
				# Mapa de registos:
				# str: $a0
				# len: $v0
	.text
	.globl strlen
strlen:
	li $v0,0		# len = 0;
for:	lb $t0,0($a0)		# $t0 = *str
	beq $t0,'\0',endf	# for( len = 0; *str != '\0'; len++; str++;){
	addiu $a0,$a0,1		# str++;
	addi $v0,$v0,1		# len++;
	j for
endf:				# }
				# return len;
	jr $ra			# Fim da função Strlen

################################ Strcpy ####################################

	# Sub-rotina terminal que copia a string localizada em "src" para "dst"
	# char *strcpy(char *dst, char *src)
				# Mapa de registos:
				# dst: $a0
				# src: $a1
				# p: $v0
	.text
	.globl strcpy
strcpy:
	move $v0,$a0		# *p = dst;
	
for2:	lb $t0,0($a1)
	sb $t0,0($a0)		# *dst = *src
	bne $t0,'\0',endf2	# for( ; (*dst = *src) != '\0'; dst++; src++){
	addiu $a0,$a0,1		# dst++;
	addiu $a1,$a1,1		# src++;
	j for2			
endf2:				# }
				# return p;
	jr $ra			# Fim da função Strcpy

################################ Strcat #####################################
	
	# Sub-rotina intermédia que concatena "dst" e "src" Strings, o resultado é guardado.
 # O resultado é guardado na String "dst".
	# char *strcat(char *dst, char *src)			
				# Mapa de registos:
				# dst: $a0 -> $s0
				# src: $a1 -> $s1
				# p: $s2
	.text
	.globl strcat
strcat:
	addiu $sp,$sp,-16	# Reservar espaço na stack
	sw $ra,0($sp)
	sw $s0,4($sp)
	sw $s1,8($sp)
	sw $s2,12($sp)

	move $s0,$a0
	move $s1,$a1
	move $s2,$a0		# *p = dst;

for3:	lb $t0,0($s0)
	beq $t0,'\0',endf3	# for( ; *dst != '\0'; dst++){

	move $a0,$s0
	move $a1,$s1
	jal strcpy		# strcpy( dst, src );
	
	addiu $s0,$s0,1		# dst++;
	j for3
endf3:				# }
	move $v0,$s2		# return p;
	lw $ra,0($sp)		
	lw $s0,4($sp)
	lw $s1,8($sp)
	lw $s2,12($sp)
	addiu $sp,$sp,16	# Libertar a stack
	jr $ra			# Fim da função strcat

################################ Strcmp ###################################

	# Sub-rotina Terminal compares two strings char by char
	# int strcmp(char *str1, char *str2)
				# Mapa de registos:
				# str1: $a0
				# str2: $a1
	.text
	.globl strcmp
strcmp:
for4:	lb $t0,0($a0)
	lb $t1,0($a1)
	bne $t0,$t1,endf4	# (*str1 == *str2)
	beq $t0,'\0',endf4	# for( ; (*str1 == *str2) && (*str1 != '\0'); str1++; str2++ ){

	addiu $a0,$a0,1		# str1++;
	addiu $a1,$a1,1		# str2++;
	j for4
endf4:				# }
	sub $v0,$t0,$t1		# return( *str1 - *str2 );

	jr $ra			# Fim do programa
