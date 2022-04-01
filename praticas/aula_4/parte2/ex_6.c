#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms);

int main(void){
	
	static const int display7Scodes[] = {0x3F, // 0
										0x06, // 1
										0x5B, // 2
										0x4F, // 3 
										0x66, // 4 
										0x6D, // 5 
										0x7D, // 6 
										0x07, // 7 
										0x7F, // 8
										0x6F, // 9 
										0x77, // a 
										0x7C, // b 
										0x39, // c 
										0x5E, // d 
										0x79, // e
										0x71  // f 
										};
	
	TRISB = TRISB | 0x000F; // RB3 to RB0 como entrada
	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída
	TRISD = TRISD & 0xFF9F;	// RD5, RD6 como saída
	TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
	
	LATDbits.LATD5 = 1;
	LATDbits.LATD6 = 0;

	int index, value;

	while(1){
		// Ler o valor dos portos RB0 - RB3;
		index = (PORTB & 0x000F); // Filtrar pelos valores bits corretos
		value = display7Scodes[index]; // Get the code for the value
		LATB = (LATB & 0x80FF) | (unsigned int) value << 8; // Limpar o ecrã e ajustar o valor do value para começar em RB8

	}
	return 0;
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}


