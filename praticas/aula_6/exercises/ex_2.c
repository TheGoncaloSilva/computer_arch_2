#include <detpic32.h>

#define K (PBCLK / 1000)

int main(void){
	volatile int aux;
	// configurar porto RB4 como entrada analógica
	TRISDbits.TRISD11 = 0;	// RD11 configurado como saída digital

	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 0;	// apenas 1 conversão consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	while(1){
		LATDbits.LATD11 = 1;	// forçar LATD11 (LATD11=1)

		AD1CON1bits.ASAM = 1;	// Iniciar a conversão
		while( IFS1bits.AD1IF == 0 );	// Esperar pela conclusão da conversão -> polling

		LATDbits.LATD11 = 0;	// Reiniciar LATD11
		aux = ADC1BUF0; // Atribuir a "aux" o valor de conversão
		printInt(aux, 16);
		putChar('\r');	// DEBUG

		IFS1bits.AD1IF = 0;		// desativar/reset manual da operação

	}

	return 0;
}
