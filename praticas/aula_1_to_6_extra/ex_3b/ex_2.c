#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(int ms);

int main(void){
	
	TRISE = (TRISE & 0xFFF0); // Configurar RE0-RE3 como saída = 0
	char c;
	while(1){
		c = inkey();

		switch(c){
			case '0': LATE = ((LATE & 0xFFF0) | 0x0001); break; // Ligar RE0
			case '1': LATE = ((LATE & 0xFFF0)  | 0x0002); break; // Ligar RE1
			case '2': LATE = ((LATE & 0xFFF0)  | 0x0004); break; // Ligar RE2
			case '3': LATE = ((LATE & 0xFFF0)  | 0x0008); break; // Ligar RE3
			case '\0': break;
			default: 
				LATE = (LATE | 0x000F); // Ligar os LEDs  RE0-RE3
				delay(1000); // Ficar ligado durante 1 segundo = 1 Hz
				LATE = (LATE & 0xFFF0); // Desligar os LEDS RE0-RE3
				break;
		}
	}
}

void delay(int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}
