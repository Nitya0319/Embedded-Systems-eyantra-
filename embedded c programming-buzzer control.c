
#include "firebird_simulation.h"		// Header file included that contains macro definitions essential for Firebird V robot
#include <util/delay.h>					// Standard AVR Delay Library

void buzzer_pin_config() {
	buzzer_ddr_reg |=  ( 1 << buzzer_pin ) ;
	buzzer_port_reg &= ~( ( 1 << buzzer_pin ) );
}

void buzzer_on(void) {
	//set buzzer to high
	buzzer_port_reg |=  ( 1 << buzzer_pin ) ;
}

void buzzer_off(void) {
	// Set 'buzzer_pin' to low
	buzzer_port_reg &= ~( ( 1 << buzzer_pin ) );
}
int main(void) {
	

	buzzer_pin_config();				// Initialize the Buzzer
	
	while (1)
	{
		buzzer_on();					// Turn ON the Buzzer
		_delay_ms(1000);				// Delay of 1 second
		
		buzzer_off();					// Turn OFF the Buzzer
		_delay_ms(1000);				// Delay of 1 second
	}
}

