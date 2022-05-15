#include <detpic32.h>
#define K (PBCLK / 1000)
#define VECTOR_T2 8
#define VECTOR_INT1 7

void _int_(VECTOR_T2) isr_T2(void) // Vetor na página 74 do family datasheet
{

}

void _int_(VECTOR_INT1) isr_INT1(void) // Vetor na página 74 do family datasheet
{

}

int main(void){
	TRISE = TRISE & 0xFFFE;	// RE0 como saída = 0

	// Ver apontamentos no tablet para cálculo de valores
	PR2 = 26042;	// Valor máximo do contador
	T2CONbits.TCKPS = 7;	// divide by 16 -> ver manul do timer (sect 14, página 14-10)
	TMR2 = 0;	// TMR2 counter = 0
	T2CONbits.ON = 1;	// enable Timer 2

	// Configurar sistema de interrupções T2
	PC2bits.T2IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T2IE = 1;	// Enable timer T1 interrupts
	IFS0bits.T2IF = 0; // Reset timer T1 interrupt flag

	// Configurar sistema de interrupções INT1
	PC1bits.INT1EP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.INT1E = 1;	// Enable timer T1 interrupts
	IFS0bits.INT1F = 0; // Reset timer T1 interrupt flag

	EnableInterrupts();
	while(1){

	}
	return 0;
}
