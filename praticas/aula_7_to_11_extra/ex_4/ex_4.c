#include <detpic32.h>
#define VECTOR_ADC 27
#define VECTOR_T5 20

volatile int VAL_ADC = 0;

void configureAdc();
void configurePwm();
void setPwm(int dutyCycle);
void send2Displays(char sendByte);
void configureDisplays();
unsigned char toBcd(unsigned char value);

void _int_(VECTOR_T5) isr_T5(void) // Vector in page 74 of family datasheet file
{
	if(VAL_ADC == 100){
		send2Displays(toBcd(00));
		LATCbits.LATC14 = 1;	// Toggle RC14 LED
	}else{
		send2Displays(toBcd(VAL_ADC));
		LATCbits.LATC14 = 0;	// Toggle RC14 LED
	}
	IFS0bits.T5IF = 0;		// Reset T5IF flag
}

void _int_(VECTOR_ADC) isr_adc(void) // Vector in page 74 of family datasheet file
{
	int *p = (int *)(&ADC1BUF0);
	int i;
	VAL_ADC = 0;
	for(i = 0; i < 4; i++){		// Calculate buffer average (4 samples)
		VAL_ADC += p[i*4];
	}
	VAL_ADC /= i;
	VAL_ADC =  (int)(100.f * ((float)VAL_ADC / 1023)); // 1023 for 2¹⁰ of resolution
	setPwm(VAL_ADC);
	IFS1bits.AD1IF = 0;		// Reset AD1IF flag
}

int main(void){
	configureAdc();
	configurePwm();
	configureDisplays();

	EnableInterrupts();
	while(1){
		AD1CON1bits.ASAM = 1;	// Start conversion
		resetCoreTimer();
		while(readCoreTimer() < 2000000);	// 10 Hz = (20 000 000 / 10)
	}
	return 0;
}

void configureAdc(){
	/* Configure ADC module */
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 4-1;	// apenas 4 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	/* Configure interrupt system */
	IPC6bits.AD1IP = 2;			// Configurar prioridade para o conversor A/D
	IFS1bits.AD1IF = 0;			// clear A/D interrupt flag
	IEC1bits.AD1IE = 1;			// enable A/D interrupts
}

void configurePwm(){
	// See tablet anotations for values
	/* Configure Timer T3 (1 KHz = 1000 Hz = 1 ms) */
	PR3 = 19999;			// Max counter value
	T3CONbits.TCKPS = 0;	// divide by 1 (000) -> Timer manual, page 14-10
	TMR3 = 0;				// TMR3 counter = 0
	T3CONbits.ON = 1;		// enable Timer 3

	/* Configure TIMER T5 values (100 Hz) -> to be used in the display  */
	PR5 = 49999;			// Max counter value
	T5CONbits.TCKPS = 2;	// divide by 4 (010) -> Timer manual (page 14-10)
	TMR5 = 0;				// Counter = 0
	T5CONbits.ON = 1;		// Enable Timer 5

	/* Configure OCM Module */
	// No need to configure pin OC1(RD2) as out, the module already does that
	OC3CONbits.OCM = 6;		// PWM mode on OCx; fault pin disabled
	OC3CONbits.OCTSEL = 1;	// Use timer T3 as the time base for PWM generation
	setPwm(0);				// Ton constant
	OC3CONbits.ON = 1;		// Enable OC1 module

	/*Configure TIMER T5 interruption system */
	IPC5bits.T5IP = 2;		// Interrupt priority (in range [1,6])
	IEC0bits.T5IE = 1;		// Enable TIMER T5 interrupts
	IFS0bits.T5IF = 0;		// Reset TIMER T5 interruption flag
}

void setPwm(int dutyCycle){
	// duty_cycle must be in the range [0, 100]
	OC3RS = (int)((PR3 + 1) * (float)dutyCycle / 100);
	printInt10(OC3RS);
	putChar('\r');
}

void configureDisplays(){
	TRISB = TRISB & 0x80FF;		// RB8-RB14 as out = 0
	TRISD = TRISD & 0xFF9F;		// RD5-RD6 as out = 0
	TRISCbits.TRISC14 = 0;		// RC14 as out = 0
}

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
}

void send2Displays(char sendByte){
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
