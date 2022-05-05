#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int main(void){
	TRISE = (TRISE & 0xFFC0); // RE5-RE0 como saída = 0
	TRISBbits.TRISB2 = 1;	// RB2 como entrada = 1

	unsigned int counter = 1;
	int freq;
	while(1){
		

		LATE = (LATE & 0xFFC0) | counter;
		counter = counter << 1;
		if(counter == 32) counter = 1;
		freq = PORTBbits.RB2;
		if(freq == 0){
			delay(333);	// 3 Hz
		}else{
			delay(142);	// 7 Hz
		}

	}
	return 0;
}
