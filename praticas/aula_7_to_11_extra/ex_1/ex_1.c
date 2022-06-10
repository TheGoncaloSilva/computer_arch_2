#include <detpic32.h>
#define VECTOR_T2 8
#define K (PBCLK / 1000)

void delay(unsigned int ms);
void send2displays(unsigned char sendValue, char status);

volatile static char displayValue = 0, displayStatus = 'D';

void _int_(VECTOR_T2) isr_T2(void) // Vector in page 74 of family datasheet file
{
	send2displays(displayValue, displayStatus); // Send the recorded value to the displays
	IFS0bits.T2IF = 0;	// Reset T2IF flag
}

void toggleLed(char value){
	LATE = LATE & 0xFFF0;	// reset RE0-RE3

	if(value == '0'){
		LATEbits.LATE0 = 1;
		send2displays(0x00, 'E');
	}else if(value == '1'){
		LATEbits.LATE1 = 1;
		send2displays(0x01, 'E');
	}else if(value == '2'){
		LATEbits.LATE2 = 1;
		send2displays(0x02, 'E');
	}else if(value == '3'){
		LATEbits.LATE3 = 1;
		send2displays(0x03, 'E');
	}else{
		LATE = LATE | 0x0001;	// All enabled
		send2displays(0xFF, 'E');
		//preciseDelay(20000000);	// 1s -> 20 MHz / 1 = 20 MHz
		delay(1000);
		send2displays(0x00, 'D');
		LATE = LATE & 0xFFF0;	// reset RE0-RE3
	}
	
}

void send2displays(unsigned char sendValue, char status){
	displayValue = sendValue;
	displayStatus = status;
	if(status == 'D'){
		LATB = LATB & 0x80FF;	// reset displays
		LATD = LATD & 0xFF9F;	// reset displays
		return;	
	}
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	static char displayFlag = 0;

	if(displayFlag){	// display high
		LATDbits.LATD5 = 0;
		LATDbits.LATD6 = 1;
		LATB = (LATB & 0x80FF) | ((unsigned int) display7Scodes[sendValue >> 4] << 8);
		
	}else{	// display low
		LATDbits.LATD5 = 1;
		LATDbits.LATD6 = 0;
		LATB = (LATB & 0x80FF) | ((unsigned int) display7Scodes[sendValue & 0x0F] << 8);

	}
	displayFlag = !displayFlag;
}

void preciseDelay(unsigned int MHz){
	resetCoreTimer();
	while(readCoreTimer() < MHz);
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int main(void){
	TRISE = TRISE & 0xFFF0;	// RE3-RE0 as out = 0
	TRISB = TRISB & 0x80FF;	// RB14-RB8 as out = 0
	TRISD = TRISD & 0xFF9F;	// RD6-RD5 as out = 0
	LATE = LATE & 0xFFF0;	// reset RE0-RE3
	LATD = LATD & 0xFF9F;	// reset display
	
	/* Configue TIMER T2 interrupt (100 Hz) -> See tablet anotations */
	PR2 = 49999;	// Max counter value
	T2CONbits.TCKPS = 2;	// divide by 4 -> Timer manual (sect 14, page 14-10)
	TMR2 = 0;	// TMR2 counter = 0
	T2CONbits.ON = 1;	// enable Timer 2

	/* Configure timer T2 interruption system */
	IPC2bits.T2IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T2IE = 1;	// Enable timer T1 interrupts
	IFS0bits.T2IF = 0; // Reset timer T1 interrupt flag

	EnableInterrupts();
	while(1){
		char c = inkey();
		if(c != '\0')
			toggleLed(c);
	}
	return 0;
}
