#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms);

int main(void){
	
	char c;

	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída
	TRISD = TRISD & 0xFFCF;	// RD5, RD6 como saída
	TRISDbits.TRISD5 = 0;
    	TRISDbits.TRISD6 = 0;
	
	LATDbits.LATD5 = 1;
	LATDbits.LATD6 = 0;

	while(1){
		c = getChar();
		switch(c){
			case 'A':case 'a': LATB = (LATB & 0x80FF) | 0x0100; break;
			case 'B':case 'b': LATB = (LATB & 0x80FF) | 0x0200; break;
			case 'C':case 'c': LATB = (LATB & 0x80FF) | 0x0400; break;
			case 'D':case 'd': LATB = (LATB & 0x80FF) | 0x0800; break;
			case 'E':case 'e': LATB = (LATB & 0x80FF) | 0x1000; break;
			case 'F':case 'f': LATB = (LATB & 0x80FF) | 0x2000; break;
			case 'G':case 'g': LATB = (LATB & 0x80FF) | 0x4000; break;
			case '0': LATD = (LATD & 0xFF9F) | 0x0020; break;
			case '1': LATD = (LATD & 0xFF9F) | 0x0040; break;
			default: break;
		}
	}
	return 0;
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}


