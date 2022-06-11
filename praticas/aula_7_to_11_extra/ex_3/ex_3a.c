#include <detpic32.h>

void configureAdc();
void configurePwm();


int main(void){
	configureAdc();
	configurePwm();


	EnableInterrupts();
	return 0;

	int i, vol = 0, cnt = 0;
	while(1){
		AD1CON1bits.ASAM = 1;	// Iniciar a conversão
		while( IFS1bits.AD1IF == 0 );	// Esperar pela conclusão da conversão -> polling

		int *p = (int *)(&ADC1BUF0);

		vol = 0;
		for( i = 0; i < 5; i++ ) { // até 5, para as 5 amostras
			vol += voltage_converter(p[i*4]);
		}
		send2displays(toBcd(vol/5)); // Ecras -> 00 - 41
		
	}
	
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

}
