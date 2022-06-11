#include <detpic32.h>

void configureAdc();
void configurePwm();
void setPwm(int dutyCycle);

int main(void){
	configureAdc();
	configurePwm();

	int i, vol = 0;
	while(1){
		AD1CON1bits.ASAM = 1;			// Initiate conversion
		while( IFS1bits.AD1IF == 0 );	// Wait for conversion to finish -> polling

		int *p = (int *)(&ADC1BUF0);

		vol = 0;
		for( i = 0; i < 4; i++ ) { // 4 samples
			vol += p[i*4];
		}
		vol /= i;
		vol = (int)(100.f * ((float)vol / 1023)); // 1023 for 2¹⁰ of resolution
		//printInt10(vol);
		//putChar('\r');
		setPwm(vol);
		IFS1bits.AD1IF == 0; 	// Force conversion reset
		resetCoreTimer();
		while(readCoreTimer() < 2000000);	// 10 Hz = (20 000 000 / 10)
	}
	return 0;
}

void configureAdc(){
	/* Configure ADC module */
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 4-1;	// apenas 4 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D
}

void configurePwm(){
	// See tablet anotations for values
	/* Configure Timer T3 (1 KHz = 1000 Hz = 1 ms) */
	PR3 = 19999;			// Max counter value
	T3CONbits.TCKPS = 0;	// divide by 1 (000) -> Timer manual, page 14-10
	TMR3 = 0;				// TMR3 counter = 0
	T3CONbits.ON = 1;		// enable Timer 3

	/* Configure OCM Module */
	// No need to configure pin OC3(RD2) as out, the module already does that
	OC3CONbits.OCM = 6;		// PWM mode on OCx; fault pin disabled
	OC3CONbits.OCTSEL = 1;	// Use timer T3 as the time base for PWM generation
	setPwm(0);				// Ton constant
	OC3CONbits.ON = 1;		// Enable OC1 module

	// No need to configure pin OC5(RD4) as out, the module already does that
	OC5CONbits.OCM = 6;		// PWM mode on OCx; fault pin disabled
	OC5CONbits.OCTSEL = 1;	// Use timer T3 as the time base for PWM generation
	setPwm(100);			// Ton constant
	OC5CONbits.ON = 1;		// Enable OC1 module

}

void setPwm(int dutyCycle){
	// duty_cycle must be in the range [0, 100]
	OC3RS = (int)((PR3 + 1) * (float)dutyCycle / 100);
	OC5RS = (int)((PR3 + 1) * (float)(100 - dutyCycle) / 100);	
	printInt10(OC3RS);
	putChar('	');
	printInt10(OC5RS);
	putChar('\r');
}
