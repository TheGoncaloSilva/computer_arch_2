#include <detpic32.h>
#define K (PBCLK / 1000)

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < K * ms);
}

int main(void)
{
	TRISE = TRISE & 0xFFFE;	// RE0 configurado como saída = 0
	TRISD = TRISD | 0x0100;	// Configurar INT1/RD8 como entrada = 1

	while(1){
		LATEbits.LATE0 = 0;
		while(PORTDbits.RD8 == 0);
		LATEbits.LATE0 = 1;
		delay(3000);	// 1000ms = 1 Hz -> 3000ms = 3 Hz
	}

	return 0;
}
