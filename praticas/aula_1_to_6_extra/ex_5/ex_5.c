#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value){
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	static int displayFlag = 0;

	if(displayFlag){
		LATDbits.LATD6 = 1;	// selecionar display high
		LATDbits.LATD5 = 0;
		
		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value >> 4] << 8);
	}else{
		LATDbits.LATD6 = 0;
		LATDbits.LATD5 = 1;	// selecionar display low

		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value & 0x0F] << 8);
	}
	displayFlag = !displayFlag;
}

int main(void){
	LATB = (LATB & 0x80FF);	// RB8-RB14 como saída = 0
	LATD = (LATD & 0xFF9F); // RD5-RD6 como saída = 0
	int counter = 0, freq = 0, i;
	char c;
	while(1){
		c = inkey();
		if(c >= '0' && c <= '4')
			freq = 2 * (1 + c - 48);

		printInt(counter, 10 | (2 << 16));

		if(c == 0x0A){ // Enter = 0x0A
			printStr(", ");
            printInt10(freq);
            printStr(" Hz");
		}

		putChar('\r');

		i = 0;
        do{
            send2displays(toBcd(freq));
            delay(20);
            i++;
        }
        while(i < (1000 / (20*freq)));

		counter = (counter + 1) % 100; // módulo 100
		delay(freq);	// 10 Hz -> 100 ms
	}
	return 0;
}
