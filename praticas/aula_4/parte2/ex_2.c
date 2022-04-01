#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms);

int main(void){
	
	unsigned char segment;

	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída
	TRISD = TRISD & 0xFFCF;	// RD5, RD6 como saída
	TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
	
	LATDbits.LATD5 = 1;
	LATDbits.LATD6 = 0;

	while(1){
		segment = 1;

		for(int i = 0; i < 7; i++){
			LATB = (LATB & 0x80FF) | segment; // enviar o valor do segment para o display
			delay(500);	// esperar 0.5s
			segment = segment << 1;
		}
		
		// Alternar a seleção do ecrã, usando xor
		


	}
	return 0;
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}


