#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int voltage_converter(int VAL_AD){
	return (VAL_AD * 33 + 511) / 1023;
}

void send2display(unsigned char value){
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	LATDbits.LATD5 = 1;	// display low ativado
	LATDbits.LATD6 = 0;	// desligar display high

	LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value & 0x0F] << 8);

}

int main(void){
	TRISB = TRISB & 0x80FF;	// RB14 to RB8 as output
    	TRISD = TRISD & 0xFF9F;	// Displays high/low as output
	TRISE = TRISE & 0xFFFD;	// RE1 como saída = 0

	// configurar porto RB4 como entrada analógica
	TRISBbits.TRISB4 = 1;	// RB4 configurado como entrada digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 2-1;	// apenas 2 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	int i, vol, dis;
	LATEbits.LATE1 = 0;	// RE1 = 0
	while(1){
		AD1CON1bits.ASAM = 1;	// Iniciar a conversão
		while( IFS1bits.AD1IF == 0 );	// Esperar pela conclusão da conversão -> polling
		LATEbits.LATE1 = !LATEbits.LATE1;	// sinalizar uma conversão

		int *p = (int *)(&ADC1BUF0);
		
		vol = 0;
		for(i = 0; i < 2; i++){ // Analisar duas conversões
			//vol += voltage_converter(p[i*4]);
			vol += p[i*4];
		}
		dis = ((vol/i) * 10) / 1023;
		if(dis == 10) dis--;
		send2display(dis);
		printInt(vol/i,10 | 3 << 16);
		putChar('\n');
		delay(200);	// 5Hz = 200 ms;
		
	}
	return 0;
}
