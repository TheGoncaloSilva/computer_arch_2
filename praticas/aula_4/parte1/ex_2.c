#include <detpic32.h>

#define K (PBCLK / 1000) // constante K dinâmica -> PBCLK é metade do FREQ

void delay(unsigned int ms); // declare the function to the compiler

int main(void){
	
	//TRISCbits.TRISR14 = 0; // RC14 configurado como output (para entrada é 1)
	TRISE = (TRISE & 0xFFF0); // RE0, RE1, RE2, RE3 confígurados como saída

	int up_counter = 0x0;
	while(1){
		delay(250); // 4Hz -> 1/4 = 0,25
		
		LATE = (LATE & 0xFFF0) | up_counter++;

		up_counter = up_counter & 0x000F;		// Impede o contador de ultrapassar 16		// up_counter = up_counter % 16 -> Alternativa
	}
	return 0;
}

void delay(unsigned int ms){
	resetCoreTimer();
	// valor default de K é 20000
	while(readCoreTimer() < K * ms);
}
