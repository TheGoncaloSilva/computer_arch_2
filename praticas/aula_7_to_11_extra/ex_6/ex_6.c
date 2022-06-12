#include <detpic32.h>

#define VECTOR_UART2 32
#define NONE 100
#define BUFFER_SIZE 16

typedef struct
{
	char values[BUFFER_SIZE]; // Storage area -> 16 values
	int nchar;	// Number of characters to be transmitted
	int posrd;	// Position of the next character to be transmitted
} t_buf;
volatile t_buf txbuf;	// buffer global

void configureUART(int baud);
void pushBuffer(char value);
void putC(char send2Byte);
void putStr(char *str);
void resetBuffer();
char getBufferValue(int pos);

void _int_(VECTOR_UART2) isr_uart2(void)	// Read character from FIFO
{
	if (IFS1bits.U2RXIF == 1)
	{
    	char c = U2RXREG;		// Read character from FIFO
		if(c == 'D'){
			putStr("#elem:");
			if(txbuf.posrd >= 10) putC('1');
			putC((txbuf.posrd % 10) + '0');
			putC('-');
			int i;
			for(i = 0; i < BUFFER_SIZE; i++){
				char value = getBufferValue(i);
				putC('0' + ((value & 0x8) >> 3));
				putC('0' + ((value & 0x4) >> 2));
				putC('0' + ((value & 0x2) >> 1));
				putC('0' + (value & 0x1));

				if(i < BUFFER_SIZE-1)
					putC(',');

			}
			putC('\n');
			resetBuffer();
		}

		IFS1bits.U2RXIF = 0;	// Clear UART2 Rx interrupt flag	
	}
}

int main(void){
	TRISB = TRISB | 0x000F;		// RB0-RB3 as input = 1
	configureUART(57600);
	resetBuffer();

	EnableInterrupts();			// Enable global Interrupts
	while(1){
		pushBuffer(PORTB & 0x000F);
		resetCoreTimer();
		while(readCoreTimer() < 13333334);	// 1.5 Hz = (20 000 000 / 1.5 )
	}
	return 0;
}

void configureUART(int baud){
	/* Configure UART2 (57600, O, 8, 1): */
	U2BRG = ((PBCLK + 8 * baud) / (16 * baud)) - 1;	// 1 - Configure BaudRate Generator -> (PBCLK / (16 * 600)) - 1;
	U2MODEbits.PDSEL = 0b10;	// 2 – 8 data bits, odd parity (10) -> page 21-6 of UART
	U2MODEbits.STSEL = 0;		// 1 STOP bit -> page 21-6 of UART
	U2STAbits.UTXEN = 1;		// 3 – Enable the trasmitter -> page 21-8
	U2STAbits.URXEN = 1;		// 3 - Enable the receiver -> page 21-8
	
	/* Configure interruption system for receiver */
	U2STAbits.URXISEL = 0b00;	// Select interrupt only for receiver
    IPC8bits.U2IP = 1;			// UART2 Priority
    IEC1bits.U2RXIE = 1;		// U2RX Interrupt Enable
    IFS1bits.U2RXIF = 0;		// U2RX Interrupt Flag
    IEC1bits.U2TXIE = 0;		// U2TX Interrupt Disable

	U2MODEbits.ON = 1;	// 4 – Enable UART2 (see register U2MODE)

}

void pushBuffer(char value){
	if(txbuf.nchar > 0 ){
		txbuf.values[txbuf.posrd] = value;
		txbuf.posrd++;
		txbuf.nchar--;
	}else{
		int i;
		for(i = 1; i < txbuf.posrd; i++)
			txbuf.values[i-1] = txbuf.values[i];
		txbuf.values[txbuf.posrd] = value;
	}

}

char getBufferValue(int pos){
	if(pos < 0 || pos > BUFFER_SIZE) return NONE;
	return txbuf.values[pos];

}

void resetBuffer(){
	int i;
	for(i = 0; i < BUFFER_SIZE; i++)
		txbuf.values[i] = 0;

	txbuf.posrd = 0;
	txbuf.nchar = BUFFER_SIZE-1;
}

void putC(char send2Byte){
	while( U2STAbits.UTXBF == 1);	// polling
	U2TXREG = send2Byte;			// copy value to UxTXREG register
}

void putStr(char *str){
	int i = 0;
	while(str[i] != '\n' || i < sizeof(str)){
		putC(str[i++]);
	}
}

