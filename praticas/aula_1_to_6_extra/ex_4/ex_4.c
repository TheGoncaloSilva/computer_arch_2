#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(int ms);
void send2displays(unsigned char value);
void turnDisplaysOff(void);
unsigned char toBcd(unsigned char value);

int main(void){
	TRISE = (TRISE & 0xFFF0); // RE0-RE3 como saída = 0
	TRISB = (TRISB & 0x80FF); // RB8-RB14 como saída = 0 -> Valores Displays de 7 segmentos
	TRISD = (TRISD & 0xFF9F); // RD5-RD6 como saída = 0 -> Enablers do Displays

	int i = 0;
	int saved = -1;
	char c; 
	while(1){
		c = inkey();
		switch(c){
			case '0': 
				LATE = (LATE & 0xFFF0) | 0x0001; // Ligar RE0
				saved = 0;
				send2displays(toBcd(0));
				break;
			case '1': 
				LATE = (LATE & 0xFFF0) | 0x0002; // Ligar RE1
				saved = 1;
				send2displays(toBcd(1));
				break;
			case '2': 
				LATE = (LATE & 0xFFF0) | 0x0004; // Ligar RE2
				saved = 2;
				send2displays(toBcd(2));
				break;
			case '3': 
				LATE = (LATE & 0xFFF0) | 0x0008; // Ligar RE3
				saved = 3;
				send2displays(toBcd(3));
				break;
			case '\0': 
				if(saved > 0){
					send2displays(toBcd(saved));
				}else{
					turnDisplaysOff();
				}
				break; // Tecla não premida
			default: 
				LATE = (LATE | 0x000F); // RE0-RE3 ligados
				for(i = 0; i < 100; i++){
					send2displays(0x0F);
					delay(10); // 10ms = 100 Hz
				} // Aguentar por 1 segundo
				LATE = (LATE & 0xFFF0); // Desligar RE0-RE3
				turnDisplaysOff();
				saved = -1;
				break;
		}

		delay(10); // Frequência de 100 Hz
	}
	return 0;

}

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value){
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	static char displayFlag = 0;
	if(displayFlag == 1){
		LATDbits.LATD6 = 1; // selecionar o display high
		LATDbits.LATD5 = 0; // Desligar display low
		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value >> 4] << 8);
				// << 8 para colocar o valor nos bits 8 a 14
				// >> 4 para colocar o valor no display high
	}else{
		LATDbits.LATD6 = 0;	// Desligar o display high
		LATDbits.LATD5 = 1;	// Selecionar o display low
		LATB = (LATB & 0x80FF) | (unsigned int) (display7Scodes[value & 0x0F] << 8);
				// << 8 para colocar o valor nos bits 8 a 14
				// & 0x0F para isolar o valor dos 4 MSBs, para ser enviado para o display low
	}
	displayFlag = !displayFlag; // Alternar o display

}

void turnDisplaysOff(void){
	LATD = (LATD & 0xFF9F); // Desligar ambos os ecrãs
}

void delay(int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);

}
