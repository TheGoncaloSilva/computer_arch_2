#include <detpic32.h>
#define K (PBCLK / 1000)
#define VECTOR 12

void _int_(VECTOR) isr_T3(void) // Vetor na página 74 do family datasheet
{
	static int flag = 0;
	flag = (flag + 1) % 2; // quando múltiplo de 2
	if(flag == 0)
		putChar('.');
	IFS0bits.T3IF = 0; // Reset timer T3 interrupt flag
}

int main(void)
{
	// Configure Timer T3 (2 Hz with interrupts disabled)
	// ver apontamentos no tablet
	PR3 = 39062;	// (PBCLK / 256) / 2 - 1;
	T3CONbits.TCKPS = 7;	// divide by 256 -> ver manul do timer (sect 14)
	TMR3 = 0;	// TMR3 counter = 0
	T3CONbits.ON = 1;	// enable Timer 3

	// Configurar sistema de interrupções
	IPC3bits.T3IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T3IE = 1;	// Enable timer T3 interrupts
	IFS0bits.T3IF = 0; // Reset timer T3 interrupt flag

	EnableInterrupts();
	while(1);
	return 0;
}
