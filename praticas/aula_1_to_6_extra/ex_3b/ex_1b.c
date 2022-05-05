#include <detpic32.h>

#define K (PBCLK / 1000)

int main(void){
	TRISB = (TRISB | 0x000F); // RB0-RB3 como entrada = 1
	TRISE = (TRISE & 0xFFF0); // RE0-RE3 como saída = 0
	while(1){
		LATE = (LATE & 0xFFF0) | ((PORTB & 0x0008) >> 3) | ((PORTB & 0x0004) >> 1) | ((PORTB & 0x0002) << 1) | ((PORTB & 0x0001) << 3);
	}
}
