#include <detpic32.h>

#define K (PBCLK / 1000)
#define nop asm volatile("nop")
#define w100ns() asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");

void sendPixel(unsigned char red, unsigned char green, unsigned char blue);
void sendByte(unsigned char byte);
void latch(void);
void delay(unsigned int ms);
void allSameColor(unsigned char red, unsigned char green, unsigned char blue, int n_pixels);
void rgb(unsigned char red, unsigned char green, unsigned char blue, int n_pixels);

int main(void){
	TRISBbits.TRISB6 = 0;
	//allSameColor(10,10,0,12);
	//rgb(10,20,30,10);
	while(1){
		//rgb(0,0,0,10);
		allSameColor(0,0,0,12);
		delay(500);
		allSameColor(10,10,0,12);
		delay(500);
		//rgb(10,20,30,10);
	}
	//while(1){
		//sendByte(0xFF);
	//}
	return 0;
}

void rgb(unsigned char red, unsigned char green, unsigned char blue, int n_pixels){
	int i;
	for(i = 0; i < n_pixels; i++){
		if(i % 3 == 0)
			sendPixel(red, 0, 0);
		if(i % 3 == 1)
			sendPixel(0, green, 0);
		if(i % 3 == 2)
			sendPixel(0, 0, blue);
	}
	latch();
}

void latch(void){
	LATB = LATB && 0xFF4F; // RB6 a 0;
	delay(1); // linha a 0 durante 1ms;

}

void allSameColor(unsigned char red, unsigned char green, unsigned char blue, int n_pixels){
	int i;
	for(i = 0; i < n_pixels; i++)
		sendPixel(red, green, blue);
	latch();
}

void sendPixel(unsigned char red, unsigned char green, unsigned char blue){
	sendByte(green);
	sendByte(red);
	sendByte(blue);
}

void sendByte(unsigned char byte){
	int i;
	for(i = 0; i < 8; i++){ // go for 1 byte
		LATB = LATB | 0x0040; // bit 6 a 1 -> linha a 1

		// 0,3 us -> 300 ns -> 300/25 = 12
		w100ns();
		w100ns(); // 8 nop -> 200 ns;

		LATBbits.LATB6 = (byte >> 7); // Isolar MSB -> O seguinte faz o mesmo;
									// linha a /1 ou /0
		//LATB = (LATB & 0xFF8F) | (byte >> 6) & 0x0040; // Isolar o bit 6 e colocar o valor 
											  			// do byte nele e descartar os outros
														// bits;
		w100ns();
		w100ns();
		w100ns();
		w100ns(); // 12 nop -> 300 ns;
		
		LATB = LATB & 0xFFBF; // colocar a linha a 0
		byte = byte << 1; // deslocar a byte para ativar os vários leds
		
	}
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}
