#include <detpic32.h>
#define K (PBCLK / 1000)
#define VECTOR_UART2 32

void configUart2(unsigned int baud, char parity, unsigned int stopbits){
	// Configure BaudRate Generator
	if(baud >= 600 && baud <= 115200)
		U2BRG = ((PBCLK + 8 * baud) / (16 * baud)) - 1;	// 	// (PBCLK / (16 * 115200)) - 1
	else	
		U2BRG = 10;	// (PBCLK / (16 * 115200)) - 1

	// Configure data bits
	if(parity == 'E')
		U2MODEbits.PDSEL = 0b01;	// 8 data bits, even parity -> page 21-6 of UART
	else if(parity == 'O')
		U2MODEbits.PDSEL = 0b10;	// 8 data bits, odd parity -> page 21-6 of UART
	else 
		U2MODEbits.PDSEL = 0b00;	// 8 data bits, no parity -> page 21-6 of UART

	// configure stop bit
	if(stopbits == 2)
		U2MODEbits.STSEL = 1;	// 2 STOP bit -> page 21-6 of UART
	else
		U2MODEbits.STSEL = 0; 	// 1 STOP bit -> page 21-6 of UART

	// Enable the trasmitter and receiver modules
	U2MODEbits.BRGH = 0;	// Divide by 16
	U2STAbits.UTXEN = 1;	// 3 – Enable the trasmitter -> page 21-8
	U2STAbits.URXEN = 1;	// 3 - Enable the receiver -> page 21-8

	U2STAbits.URXISEL = 0b00;	// Select interrupt only for receiver
    IPC8bits.U2IP = 1;	// UART2 Priority
    IEC1bits.U2RXIE = 1;	// U2RX Interrupt Enable
    IFS1bits.U2RXIF = 0;	// U2RX Interrupt Flag
    IEC1bits.U2TXIE = 0;	// U2TX Interrupt Disable

	// Enable UART2
	U2MODEbits.ON = 1;	// 4 – Enable UART2 (see register U2MODE)
}

void putc(char byte2send) {
	while( U2STAbits.UTXBF == 1 );	// wait while UTXBF == 1
	U2TXREG = byte2send;	// Copy byte2send to the UxTXREG register
}

void _int_(VECTOR_UART2) isr_uart2(void)
{
	if (IFS1bits.U2RXIF == 1)
	{
		char byte = U2RXREG;	// Read character from FIFO (U2RXREG)
		putc(byte);		// Send the character using putc()
    	IFS1bits.U2RXIF = 0;	// Clear UART2 Rx interrupt flag
	}
}

int main(void){
	
	configUart2(115200, 'N', 1);	// Configure UART: 115200, N, 8, 1
	
	EnableInterrupts();	// Enable global Interrupts
	while(1);
	return 0;
}
