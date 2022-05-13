#include <detpic32.h>
#define K (PBCLK / 1000)

int main(void)
{
	// Configure Timer T3 (2 Hz with interrupts disabled)
	// ver apontamentos no tablet
	PR3 = 39062;	// (PBCLK / 256) / 2 - 1;
	T3CONbits.TCKPS = 7;	// divide by 256 -> ver manul do timer (sect 14)
	TMR3 = 0;	// TMR3 counter = 0
	T3CONbits.ON = 1;	// enable Timer 3

	while(1)
	{
		while( IFS0bits.T3IF == 0 );	// Wait while T3IF = 0
		putChar('.');
		IFS0bits.T3IF = 0;	// Reset T3IF
	}
	return 0;
}
