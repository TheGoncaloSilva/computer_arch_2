#include <detpic32.h>
#define K (PBCLK / 1000)

int main(void){
	
	// Configure UART2:
	U2BRG = 10;	// 1 - Configure BaudRate Generator -> (PBCLK / (16 * 115200)) - 1;
	U2MODEbits.PDSEL = 0b00;	// 2 – 8 data bits, no parity (00) -> page 21-6 of UART
	U2MODEbits.STSEL = 0;	// 1 STOP bit -> page 21-6 of UART
	U2STAbits.UTXEN = 1;	// 3 – Enable the trasmitter -> page 21-8
	U2STAbits.URXEN = 1;	// 3 - Enable the receiver -> page 21-8
	U2MODEbits.ON = 1;	// 4 – Enable UART2 (see register U2MODE)

	return 0;
}
