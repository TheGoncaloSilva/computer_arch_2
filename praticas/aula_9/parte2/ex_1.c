#include <detpic32.h>

void configureAll();

int main(void){
	configureAll();

	while(1);
	return 0;
}

void configureAll(){
	// Ver apontamentos tablet para valores
	// Configurar Timer T3 (100 Hz)
	PR3 = 49999;	// Valor máximo do contador
	T3CONbits.TCKPS = 2;	// divide by 4 -> ver manul do timer (sect 14, página 14-10)
	TMR3 = 0;	// TMR3 counter = 0
	T3CONbits.ON = 1;	// enable Timer 3

	// Configurar OCM
	// Não é preciso configurar OC1(RD0) como saída, o módulo já efetua isso
	OC1CONbits.OCM = 6;	// PWM mode on OCx; fault pin disabled
	OC1CONbits.OCTSEL = 1;	// Use timer T3 as the time base for PWM generation
	OC1RS = 12500;	// Ton constant
	OC1CONbits.ON = 1;	// Enable OC1 module

	/* Não existe sistema de interrupções, é inteiramente hardware */
}
