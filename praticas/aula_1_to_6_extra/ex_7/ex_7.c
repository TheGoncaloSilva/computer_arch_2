#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int main(void){
	TRISB = (TRISB | 0x000F); // RB0-RB3 como entrada = 0
	while(1){
		printStr("DS4=");
		putChar(48 + PORTBbits.RB3);
		printStr(", ");

		printStr("DS3=");
		putChar(48 + PORTBbits.RB2);
		printStr(", ");

		printStr("DS2=");
		putChar(48 + PORTBbits.RB1);
		printStr(", ");

		printStr("DS1=");
		putChar(48 + PORTBbits.RB0);
		printStr(", ");

		putChar('\r');
		delay(1000); // 1 Hz
	}
	return 0;
}
