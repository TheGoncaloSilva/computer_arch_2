#include <detpic32.h>

void delay(unsigned int ms); // declare the function to the compiler

int main(void){

	int counter_1 = 0, counter_5 = 0, counter_10 = 0;

	while(1)
	{	
		printInt(counter_1, 10 | 4 << 16);
		putChar('\t'); // tab
		printInt(counter_5, 10 | 4 << 16);
		putChar('\t');
		printInt(counter_10, 10 | 4 << 16);
		putChar('\r');	// cursor regressa ao inicio da linha
		
		if(counter_10 % 10 == 0) 
			counter_1++;
		if(counter_10 % 2 == 0)
			counter_5++;
			
		counter_10++;
		delay(100); // 0,1s de atraso
	}
	return 0;	
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < 20000 * ms);
}
