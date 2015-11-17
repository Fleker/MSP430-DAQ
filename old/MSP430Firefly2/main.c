#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    char *Flash_ptr = (char *) 0x1040; //Flash pointer
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ; //Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;
	FCTL2 = FWKEY + FSSEL0 + FN1; //MCLK/3 for Flash Timing Generator

	__no_operation(); //Place breakpoint here, observe 0x1040
	FCTL3 = FWKEY; ///Clear Lock bit
	FCTL1 = FWKEY + WRT; //Set WRT bit for write operation

	*Flash_ptr = 0xAA; //Write value to flash
	__no_operation(); //Place breakpoint here, observe 0x1040
	*Flash_ptr = 0xFF;	//Write value to flash. Fails because
					   	//we can't set bits to '1'

	__no_operation(); //Place breakpoint here, observe 0x1040
	*Flash_ptr = 0x00; //Write value to flash, All bits set to 0.
	__no_operation(); // Place breakpoint here, observe 0x1040 with 0x00
	//Ensure we don't exit from program
	while(1) {
		__no_operation();
	}
	
	//return 0;
}
