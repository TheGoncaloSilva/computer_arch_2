#include <detpic32.h>

#define K (PBCLK / 1000) // constante K dinâmica -> PBCLK é metade do FREQ

void delay(unsigned int ms); // declare the function to the compiler

int main(void){
	
	// Configure port RC14 as output
	TRISCbits.TRISC14 = 0; // RC14 configurado comon output (para entrada é 1)
	
	while(1){
		delay(500); // wait 0.5s
		// toggle RC14 port value
		LATCbits.LATC14 = !LATCbits.LATC14; // Atribuir ao porto RC14
						    // a negação do mesmo
	
	}
	return 0;
}

void delay(unsigned int ms){
	resetCoreTimer();
	// valor default de K é 20000
	while(readCoreTimer() < K * ms);
}
