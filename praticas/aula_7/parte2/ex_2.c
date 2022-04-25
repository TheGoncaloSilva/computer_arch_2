// Com o _int_ o compilador automaticamente cria o prólogo e o epílogo 
#include <detpic32.h>

#define K (PBCLK / 1000)
#define VECTOR 27 // VECTOR está definido na página 75 do PIC32 family

volatile int adc_value;

// Interrupt service routine (interrupt handler)
void _int_(VECTOR) isr_adc(void) // Replace VECTOR by the A/D vector
								// number - see "PIC32 family data
								// sheet" (pages 74-76)
{
	int time = readCoreTimer();
	LATDbits.LATD11 = 0;	// Reset RD11
	// Read conversion result (ADC1BUF0) and print it
	adc_value = *(int*)(&ADC1BUF0);

	time *= 50;				// convert to ns(PBCLK = 20 Mhz = 50 ns)
	time -= 3600;
	printInt10(time);		// Entre 1100-1150
	putChar('\r');
	
	resetCoreTimer();
    AD1CON1bits.ASAM = 1;	// Start A/D conversion
	LATDbits.LATD11 = 1;	// Set RD11
	IFS1bits.AD1IF = 0;		// Reset AD1IF flag
}

int main(void)
{
	// Configure all (digital I/O, analog input, A/D module)
	TRISDbits.TRISD11 = 1;	// RD11 configurado como saída
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 16;		// Tempo de sample é 16 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 0;	// apenas 4 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	// Configure interrupt system
	IPC6bits.AD1IP = 2;		// Configurar prioridade para o conversor A/D
	IFS1bits.AD1IF = 0;		// clear A/D interrupt flag
	IEC1bits.AD1IE = 1;		// enable A/D interrupts

	EnableInterrupts();		// Global Interrupt Enable
	
	AD1CON1bits.ASAM = 1;	// Iniciar a conversão

	resetCoreTimer();

	while(1);
	// all activity is done by the ISR
	return 0;
}
