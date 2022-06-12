#include <detpic32.h>

void configureUART();
void putc(char byte2send);
void putstr(char *str);

int main(void){
	/* Run with: pterm 1200,N,8,1 */
	TRISB = TRISB | 0x000F;		// RB0-RB3 as input = 1
	configureUART();
	while(1){
		putstr("RB30=");
		putc(PORTBbits.RB3 + '0');
    	putc(PORTBbits.RB2 + '0');
    	putc(PORTBbits.RB1 + '0');
    	putc(PORTBbits.RB0 + '0');
		putc('\n');

		resetCoreTimer();
		while(readCoreTimer() < 10000000);	// 2Hz = (20 000 000 / 2)
	}
	return 0;
}

void configureUART(){
	// Configure UART2 (1200, N, 8, 1):
	U2BRG = ((PBCLK + 8 * 1200) / (16 * 1200)) - 1;	// 1 - Configure BaudRate Generator -> (PBCLK / (16 * 600)) - 1;
	U2MODEbits.PDSEL = 0b00;	// 2 – 8 data bits, no parity (00) -> page 21-6 of UART
	U2MODEbits.STSEL = 0;	// 1 STOP bit -> page 21-6 of UART
	U2STAbits.UTXEN = 1;	// 3 – Enable the trasmitter -> page 21-8
	U2STAbits.URXEN = 0;	// 3 - Enable the receiver -> page 21-8
	U2MODEbits.ON = 1;	// 4 – Enable UART2 (see register U2MODE)
}

void putc(char byte2send) {
	while( U2STAbits.UTXBF == 1 );	// wait while UTXBF == 1
	U2TXREG = byte2send;	// Copy byte2send to the UxTXREG register
}

void putstr(char *str){
	int i = 0;
	while(str[i] != '\n' || i < sizeof(str)){
		putc(str[i++]);
	}
}

