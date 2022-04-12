#include <detpic32.h>

#define K (PBCLK / 1000)

int main(void){
	// configurar porto RB4 como entrada analógica
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 1;	// apenas 1 conversão consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	while(1){
		AD1CON1bits.ASAM = 1;	// Iniciar a conversão
		while( IFS1bits.AD1IF == 0 );	// Esperar pela conclusão da conversão -> polling
		printInt(ADC1BUF0, 16 | 3 << 16);	// Imprimir o resultado da conversão
		putChar('\r');
		IFS1bits.AD1IF = 0;		// desativar/reset manual da operação

	}

	return 0;
}
