#include <detpic32.h>
#define VECTOR_T1 4
#define VECTOR_T2 8
#define VECTOR_ADC 27

volatile int counter = 0;
volatile int maxValue = 100; // Module or max value of the counter
volatile int freq = 2;

void configure_timers();
void configure_displays();
void send2Displays(unsigned char sendByte);
unsigned char toBcd(unsigned char value);
int reconfigure_timer1(int freq);
int converter(int VALD_AD);
void configure_adc();

void _int_(VECTOR_T1) isr_T1(void) // Vector in page 74 of family datasheet file
{
	counter = (counter + 1) % maxValue; // increment counter
	IFS0bits.T1IF = 0;		// Reset T1IF flag
}

void _int_(VECTOR_T2) isr_T2(void) // Vector in page 74 of family datasheet file
{
	send2Displays(toBcd(counter)); // send value to displays
	/*printInt(counter, 16 | 2 << 16);
	putChar('\r');*/
	IFS0bits.T2IF = 0;		// Reset T2IF flag
}

void _int_(VECTOR_ADC) isr_adc(void) // Vector in page 74 of family datasheet file
{
	int *p = (int *)(&ADC1BUF0);
	int valAdc = 0, i;
	for(i = 0; i < 4; i++){		// Calculate buffer average (4 samples)
		valAdc += p[i*4];
	}
	valAdc /= i;
	freq = reconfigure_timer1(valAdc);
	IFS1bits.AD1IF = 0;		// Reset AD1IF flag
}

int main(void){
	configure_displays();
	configure_timers();
	configure_adc();

	EnableInterrupts();		// Enable global interrupts
	while(1){
		resetCoreTimer();
		while(readCoreTimer() < 5000000); 	// 4Hz = (20 000 000 / 4)

		AD1CON1bits.ASAM = 1;				// Start A/D conversion
	}
	return 0;
}

void configure_timers(){
	/* Configure TIMER T1 values (10 Hz) -> to be used in the counter  */
	PR1 = 31249;			// Max counter value
	T1CONbits.TCKPS = 2;	// divide by 64 (10) -> Timer manual (page 14-8)
	TMR1 = 0;				// Counter = 0
	T1CONbits.ON = 1;		// Enable Timer 1

	/* Configure TIMER T2 values (50 Hz) -> to be used in the display  */
	PR2 = 49999;			// Max counter value
	T2CONbits.TCKPS = 3;	// divide by 8 (011) -> Timer manual (page 14-10)
	TMR2 = 0;				// Counter = 0
	T2CONbits.ON = 1;		// Enable Timer 2

	/*Configure TIMER T1 interruption system */
	IPC1bits.T1IP = 2;		// Interrupt priority (in range [1,6])
	IEC0bits.T1IE = 1;		// Enable TIMER T1 interrupts
	IFS0bits.T1IF = 0;		// Reset TIMER T1 interruption flag

	/*Configure TIMER T2 interruption system */
	IPC2bits.T2IP = 2;		// Interrupt priority (in range [1,6])
	IEC0bits.T2IE = 1;		// Enable TIMER T2 interrupts
	IFS0bits.T2IF = 0;		// Reset TIMER T2 interruption flag
}

void configure_displays(){
	TRISB = TRISB & 0x80FF;	// RD8-RD14 as exit = 0
	TRISD = TRISD & 0xFF9f;	// RD5-RD6 as exit = 0
	LATB = LATD & 0x80FF;	// Reset Display values
	LATD = LATD & 0xFF9F;	// Reset Display status (enables)
}

void configure_adc(){
	// Configure all (digital I/O, analog input, A/D module)
	TRISBbits.TRISB4 = 1;		// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;		// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;		// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 8;		// Tempo de sample é 8 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 0;		// apenas 4 conversões consecutiva
	AD1CHSbits.CH0SA = 4;		// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;			// Ativar o conversor A/D

	// Configure interrupt system
	IPC6bits.AD1IP = 2;			// Configurar prioridade para o conversor A/D
	IFS1bits.AD1IF = 0;			// clear A/D interrupt flag
	IEC1bits.AD1IE = 1;			// enable A/D interrupts
}

void send2Displays(unsigned char sendByte){
	static const int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	static char displayFlag = 0;

	if(displayFlag){ 		// Select Display high
		LATDbits.LATD5 = 0;
		LATDbits.LATD6 = 1;
		LATB = (LATB & 0x80FF) | (unsigned int) display7Scodes[sendByte >> 4] << 8;

	}else{
		LATDbits.LATD5 = 1;
		LATDbits.LATD6 = 0;
		LATB = (LATB & 0x80FF) | (unsigned int) display7Scodes[sendByte & 0x0F] << 8;
	}
	displayFlag = !displayFlag;
}

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
}

int reconfigure_timer1(int freq){
	freq = 1 + (freq / 127);

	/* Configure TIMER T1 values (10 Hz)*/
	if(freq == 2) {
		PR1 = 39062;			// Max counter value
		T1CONbits.TCKPS = 3;	// divide by 256 (11) -> Timer manual (page 14-8)
	}else if(freq == 4) {
		PR1 = 19531;			// Max counter value
		T1CONbits.TCKPS = 3;	// divide by 256 (11) -> Timer manual (page 14-8)
	}else if(freq == 6) {
		PR1 = 52083;			// Max counter value
		T1CONbits.TCKPS = 2;	// divide by 64 (10) -> Timer manual (page 14-8)
	}else if(freq == 8) {
		PR1 = 39062;			// Max counter value
		T1CONbits.TCKPS = 2;	// divide by 64 (10) -> Timer manual (page 14-8)
	}else{
		PR1 = 31249;			// Max counter value
		T1CONbits.TCKPS = 2;	// divide by 64 (10) -> Timer manual (page 14-8)
	}
	TMR1 = 0;					// Counter = 0
	T1CONbits.ON = 1;			// Enable Timer 1

	/*Configure TIMER T1 interruption system */
	IPC1bits.T1IP = 2;			// Interrupt priority (in range [1,6])
	IEC0bits.T1IE = 1;			// Enable TIMER T1 interrupts
	IFS0bits.T1IF = 0;			// Reset TIMER T1 interruption flag

	return freq;
}

int converter(int VALD_AD){
	return 1 + (VALD_AD / 127);
}
