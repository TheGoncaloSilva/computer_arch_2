#include <detpic32.h>
#define K (PBCLK / 1000)
#define VECTOR_T3 12
#define VECTOR_T1 4

void _int_(VECTOR_T1) isr_T1(void) // Vetor na página 74 do family datasheet
{
	putChar('1');
	LATDbits.LATD0 = !LATDbits.LATD0;	// Toggle do RD0
	IFS0bits.T1IF = 0; // Reset timer T1 interrupt flag
}

void _int_(VECTOR_T3) isr_T3(void) // Vetor na página 74 do family datasheet
{
	putChar('3');
	LATDbits.LATD2 = !LATDbits.LATD2;	// Toggle do RD2
	IFS0bits.T3IF = 0; // Reset timer T3 interrupt flag
}

int main(void)
{
	// Configurar RD0 e RD2 como saída
	TRISD = TRISD & 0xFFFA;	// RD0 e RD2 como saída = 0

	// Ver apontamentos no tablet
	// Configurar Timer T1 (5 Hz)
	PR1 = 62499;	// Valor máximo do contador
	T1CONbits.TCKPS = 2;	// divide by 64 -> ver manul do timer (sect 14, página 14-8)
	TMR1 = 0;	// TMR1 counter = 0
	T1CONbits.ON = 1;	// enable Timer 1

	// Configurar Timer T3 (25 Hz)
	PR3 = 50000;	// Valor máximo do contador
	T3CONbits.TCKPS = 4;	// divide by 16 -> ver manul do timer (sect 14, página 14-10)
	TMR3 = 0;	// TMR3 counter = 0
	T3CONbits.ON = 1;	// enable Timer 3

	// Configurar sistema de interrupções T1
	IPC1bits.T1IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T1IE = 1;	// Enable timer T1 interrupts
	IFS0bits.T1IF = 0; // Reset timer T1 interrupt flag

	// Configurar sistema de interrupções T3
	IPC3bits.T3IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T3IE = 1;	// Enable timer T3 interrupts
	IFS0bits.T3IF = 0; // Reset timer T3 interrupt flag

	EnableInterrupts();
	while(1);
	return 0;
}
