#include <detpic32.h>
#define K (PBCLK / 1000)

void putc(char byte2send) {
	while( U1STAbits.UTXBF == 1 );	// wait while UTXBF == 1
	U1TXREG = byte2send;	// Copy byte2send to the UxTXREG register
}

void putstr(char *str){
	int i = 0;
	while(str[i] != '\n' || i < sizeof(str)){
		putc(str[i++]);
	}
}

char getc(void) {
	//if(U2STAbits.OERR == 1) U2STAbits.OERR = 0;		// If OERR == 1 then reset OERR
	while(U1STAbits.URXDA == 0);	// Wait while URXDA == 0
	return U1RXREG;		// Return U2RXREG
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int main(void){
	
	// Configure UART2:
	U1BRG = 10;	// 1 - Configure BaudRate Generator -> (PBCLK / (16 * 115200)) - 1;
	U1MODEbits.PDSEL = 0b00;	// 2 – 8 data bits, no parity (00) -> page 21-6 of UART
	U1MODEbits.STSEL = 0;	// 1 STOP bit -> page 21-6 of UART
	U1STAbits.UTXEN = 1;	// 3 – Enable the trasmitter -> page 21-8
	U1STAbits.URXEN = 1;	// 3 - Enable the receiver -> page 21-8
	U1MODEbits.ON = 1;	// 4 – Enable UART2 (see register U2MODE)

	while(1){
		putc(0x5A); //0101 1010
		delay(1);	// delay 10 mill
	}
	return 0;
}
