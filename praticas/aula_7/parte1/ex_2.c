// Com o _int_ o compilador automaticamente cria o prólogo e o epílogo 
#include <detpic32.h>

#define K (PBCLK / 1000)
#define VECTOR 27 // VECTOR está definido na página 75 do PIC32 family

volatile unsigned char voltage = 0;		// Variável global (Vai sempre buscar o valor da variável à mesma
										// e não a guarda em "cache"/registo)

// Interrupt service routine (interrupt handler)
void _int_(VECTOR) isr_adc(void) // Replace VECTOR by the A/D vector
								// number - see "PIC32 family data
								// sheet" (pages 74-76)
{
	// Read conversion result (ADC1BUF0) and print it
	int value = *(int*)(&ADC1BUF0);
    printInt(value, 10 | 4 << 16);
	putChar('\r');

	voltage =( value * 33 + 511) / 1023;
	
	IFS1bits.AD1IF = 0;		// Reset AD1IF flag
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);

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

unsigned char toBcd(unsigned char value){
	return ((value / 10) << 4) + (value % 10);
}

int main(void)
{
	unsigned int cnt = 0;
	// Ativar displays
	TRISB = TRISB & 0x80FF; // RB8-RB14 como saída = 0
	TRISD = TRISD & 0xFF9F; // RD5-RD6 como saída = 0
	TRISE = TRISE & 0xFF00; // R30-RE7 como saída = 0

	// Configure all (digital I/O, analog input, A/D module)
	TRISBbits.TRISB4 = 1;	// RB4 configurado como saída digital
	AD1PCFGbits.PCFG4 = 0;	// RB4 configurado como analógico -> AN4
	AD1CON1bits.SSRC = 7;	// selecionar os bits, de forma a quando forem alterados, a conversão comece

	AD1CON1bits.CLRASAM = 1;	// Terminar a conversão quando é gerado a 1ª interrupção A/D

	AD1CON3bits.SAMC = 8;		// Tempo de sample é 8 TAB ou (TAB = 100ns)

	AD1CON2bits.SMPI = 0;	// apenas 4 conversões consecutiva
	AD1CHSbits.CH0SA = 4;	// selecionar AN4 como input do ADC

	AD1CON1bits.ON = 1;		// Ativar o conversor A/D

	// Configure interrupt system
	IPC6bits.AD1IP = 2;		// Configurar prioridade para o conversor A/D
	IFS1bits.AD1IF = 0;		// clear A/D interrupt flag
	IEC1bits.AD1IE = 1;		// enable A/D interrupts

	EnableInterrupts();		// Global Interrupt Enable
	
	while(1){
		if(cnt == 0)	// 0, 200 ms, 400 ms, ... (5 samples/second)
		{
			AD1CON1bits.ASAM = 1;	// Iniciar a conversão
		}
		// Send "voltage" value to displays
		cnt = (cnt + 1) % 20;	// 10 * 20 = 200ms -> 5Hz
		send2displays(toBcd(voltage));
		delay(10);	// Wait 10ms -> 100 Hz

	}
	
	return 0;
}
