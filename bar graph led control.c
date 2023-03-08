
 // To toggle the status of 2 Bar-graph LEDs depending on whether the Interrupt Switch is pressed or released.
#include "firebird_simulation.h"			// Header file included that contains macro definitions essential for Firebird V robot
#include <util/delay.h>						
#include <stdbool.h>						

void bar_graph_led_pins_config(void) {
	 bar_graph_led_ddr_reg        |= 0xA2;
	bar_graph_led_port_reg        |= 0x02;
}
void interrupt_sw_pin_config(void) {
           interrupt_sw_ddr_reg        &= ~( 1 << interrupt_sw_pin );

           interrupt_sw_port_reg        |= ( 1 << interrupt_sw_pin );
}
bool interrupt_switch_pressed(void)
{

 if ((interrupt_sw_pin_reg & (1 << interrupt_sw_pin)) == (1 << interrupt_sw_pin) ) {
                                                return false;        
                } else {
                return true;
        }
}
void turn_on_bar_graph_led(unsigned char led_pin) {

  bar_graph_led_port_reg  |= ( 1 << led_pin );
}

void turn_off_bar_graph_led(unsigned char led_pin) {
 bar_graph_led_port_reg &= ~( 1 << led_pin);
}

int main(void) {
	
	
	bar_graph_led_pins_config();									// Initialize the 2nd, 6th and 8th Bar-graph LEDs
	interrupt_sw_pin_config();										// Initialize the Interrupt Switch
	
	turn_on_bar_graph_led(bar_graph_led_6_pin);						// Turn ON 6th Bar-graph LED
	
	while (1)
	{
		if ( interrupt_switch_pressed() )
		{
			turn_off_bar_graph_led(bar_graph_led_2_pin);			// Turn OFF 2nd Bar-graph LED
			turn_on_bar_graph_led(bar_graph_led_8_pin);				// Turn ON 8th Bar-graph LED
			
			_delay_ms(50);											// Waiting for a while for the Interrupt Switch to settle
																	//to accomodate switch bounce
		
		}
		else
		{
			turn_on_bar_graph_led(bar_graph_led_2_pin);				// Turn ON 2nd Bar-graph LED
			turn_off_bar_graph_led(bar_graph_led_8_pin);			// Turn OFF 8th Bar-graph LED
		}
	}
}
