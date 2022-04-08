#include <detpic32.h>

#define K (PBCLK / 1000)

void delay(unsigned int ms);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

int main(void){
	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída = 0
	TRISB = TRISB | 0x0001; // RB0 como entrada = 1
	TRISD = TRISD & 0xFF9F; // RD5-RD6 como saída = 0
	TRISE = TRISE & 0xFF00; // R30-RE7 como saída = 0
	
	int i, dir, max_val;
	unsigned char counter = 0;
	while(1){
		i = 0;

		dir = (PORTB & 0x0001); // Filtrar pelo RB0
		if(dir == 1){
			counter++; // SW on
			counter = counter % 60; // mod 60
		}else{ 
			if(counter == 0) counter = 59;
			else counter--; // SW off
		}

		do {
			send2displays(toBcd(counter)); // Vai escrever 15 -> 0001 1111
			LATE = (LATE & 0xFF00) | toBcd(counter); // 0xFF00 para limpar o valor dos 8 LSB
			delay(10); // 100 Hz = 10 ms
						// 2 Hz = 500 ms
						// i vai incrementar a uma frequência de 100Hz até 50, que equivale a 2Hz
			if(dir == 1)
				max_val = 20; // Ascendente 5 Hz
			else
				max_val = 50; // Descendente 2 Hz

		} while(++i < max_val);
	}
	return 0;
}

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
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
