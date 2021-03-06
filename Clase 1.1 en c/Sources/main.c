#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

void delay (){
	unsigned int c;
	
	
	for (c=0;c<=50000;c++){
		
		asm(nop);
		
		asm(nop);
	}
	
	
}



void main(void) {
  EnableInterrupts;
  /* include your code here */

  PTCDD = 0X01;

  for(;;) {
	  
	 PTCD = 1;
	 delay();
	 PTCD = 0;
	 delay();
	 
    __RESET_WATCHDOG();	/* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
