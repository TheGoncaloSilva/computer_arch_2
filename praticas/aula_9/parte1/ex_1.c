#include <detpic32.h>

#define K (PBCLK / 1000)
#define VECTOR_T3 12
#define VECTOR_T1 4
#define VECTOR_ADC 27

volatile int voltage = 0;	// Global variable

int voltage_converter(int val_AD);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);
void configureAll();

void _int_(VECTOR_T1) isr_T1(void) // Vetor na página 74 do family datasheet
{
	AD1CON1bits.ASAM = 1;	// Start A/D conversion
	IFS0bits.T1IF = 0;	// Reset T1IF flag
}

void _int_(VECTOR_T3) isr_T3(void) // Vetor na página 74 do family datasheet
{
	send2displays(voltage); // Ecras -> 00 - 33
								// Send "voltage" value to displays (global variable)
	IFS0bits.T3IF = 0;	// Reset T3IF flag
}

void _int_(VECTOR_ADC) isr_adc(void) /// Vetor na página 74 do family datasheet
{
	int *p = (int *)(&ADC1BUF0);

	int vol = 0, i;
	for( i = 0; i < 8; i++ ) {	// Calculate buffer average (8 samples)
		vol += p[i*4];
	}
	vol /= 8;	// Average value
	voltage = voltage_converter(vol);	// Calculate voltage amplitude
	voltage = toBcd(voltage);	// Convert voltage amplitude to decimal. Copy it to "voltage"
	IFS1bits.AD1IF = 0; // Reset AD1IF flag
}

int main(void){

	configureAll();		// Function to configure all (digital I/O, analog
						// input, A/D module, timers T1 and T3, interrupts)

	IFS0bits.T1IF = 0; // Reset timer T1 interrupt flag
	IFS0bits.T3IF = 0; // Reset timer T3 interrupt flag
	IFS1bits.AD1IF = 0; // reset A/D interrupt flag

	EnableInterrupts();

	while(1);
	return 0;
}

int voltage_converter(int val_AD){
	return (val_AD * 33 + 511) / 1023;
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

void configureAll(){
	// Configurar displays
	TRISB = TRISB & 0x80FF;         // RB14 to RB8 as output
    	TRISD = TRISD & 0xFF9F;         // Displays high/low as output

	// Configurar módulo ADC
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece
	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D
	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)
	AD1CON2bits.SMPI = 8-1;	// apenas 8 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC
	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	// Ver apontamentos no tablet
	// Configurar Timer T1 (5 Hz)
	PR1 = 62499;	// Valor máximo do contador
	T1CONbits.TCKPS = 2;	// divide by 64 -> ver manul do timer (sect 14, página 14-8)
	TMR1 = 0;	// TMR1 counter = 0
	T1CONbits.ON = 1;	// enable Timer 1

	// Configurar Timer T3 (100 Hz)
	PR3 = 49999;	// Valor máximo do contador
	T3CONbits.TCKPS = 2;	// divide by 4 -> ver manul do timer (sect 14, página 14-10)
	TMR3 = 0;	// TMR3 counter = 0
	T3CONbits.ON = 1;	// enable Timer 3

	// Configurar sistema de interrupções T1
	IPC1bits.T1IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T1IE = 1;	// Enable timer T1 interrupts

	// Configurar sistema de interrupções T3
	IPC3bits.T3IP = 2; // Interrupt priority (must be in range [1..6])
	IEC0bits.T3IE = 1;	// Enable timer T3 interrupts

	// Configurar sistema de interrupções ADC
	IPC6bits.AD1IP = 2;		// Configurar prioridade para o conversor A/D
	IEC1bits.AD1IE = 1;		// enable A/D interrupts

}
