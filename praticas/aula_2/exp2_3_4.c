#include <detpic32.h>

void delay(unsigned int ms); // declare the functio to the compiler

int main(void){

	int counter_1 = 0, counter_5 = 0, counter_10 = 0,
	    speed = 100, status = 0;

	while(1)
	{	
		char key = inkey();
		if(key == 'A')
			speed = speed / 2;
		else if(key == 'N')
			speed = 100;
		else if(key == 'R'){
			counter_1 = 0;
			counter_5 = 0;
			counter_10 = 0;
			status = 0;
		}else if(key == 'S' && status == 0)
			status = 1;
		else if(key == 'S' && status == 1)
			status = 0;

		if(status == 0){

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
			delay(speed); // 100 = 0,1s de atraso
		}
	}
	return 0;	
}

void delay(unsigned int ms){
	resetCoreTimer();
	while(readCoreTimer() < 20000 * ms);
}
