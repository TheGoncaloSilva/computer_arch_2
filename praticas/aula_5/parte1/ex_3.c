#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms);
void send2displays(unsigned char value);

int main(void){
	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída = 0
	TRISD = TRISD & 0xFF9F; // RD5-RD6 como saída = 0

	while(1){
		send2displays(0x15); // Vai escrever 15 -> 0001 1111
		delay(10); // 0,2s = 5 Hz = 4 000 000; ms = 200;
					// 200 Hz = 5 ms
					// 100 Hz = 10 ms
					// 50 Hz = 20 ms
					// 20 Hz = 50 ms
					// 5 Hz = 200 ms

	}
	return 0;
}

void send2displays(unsigned char value){
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	static char displayFlag = 0; // static variable: doesn't loose its
								// value between calls to function
	if(displayFlag == 1){ // send value to display high 
		LATDbits.LATD6 = 1;		// select display high -> RD6
		LATDbits.LATD5 = 0;
		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value >> 4] << 8); // index = value >> 4; 
								// << 8 é usado para colocar o valor nos bits correspondentes a RB14 - RB8
								// send digit_high (dh) to display:		dh = value >> 4
	}else{
		LATDbits.LATD6 = 0;		// select display low -> RD5
		LATDbits.LATD5 = 1;
		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value & 0x0F] << 8); // index = value & 0x0F; 								// << 8 é usado para colocar o valor nos bits correspondentes a RB14 - RB8
	} 
	displayFlag = !displayFlag;	// alternar a variável displayFlag
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);

}
