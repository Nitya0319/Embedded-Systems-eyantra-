//Displaying characters on lcd
 /*
 *
 * LCD Connections:								<br>
 *		 LCD Pins	  Micro-controller Pins     <br>
 *			  RS  --> PB0						<br>
 *			  RW  --> PB2						<br>
 *			  EN  --> PB1						<br>
 *			  DB7 --> PD7						<br>
 *			  DB6 --> PD6						<br>
 *			  DB5 --> PD5						<br>
 *			  DB4 --> PD4						<br>
 *
 */


#include "firebird_simulation.h"		// Header file included that contains macro definitions essential for Firebird V robot and Simulation
#include <util/delay.h>					// Standard AVR Delay Library
#include "lcd.h"						// LCD Header file included that contains function definitions essential to deal with LCD

unsigned char pattern_e[8] = {
	0b00011111,						
	0b00010001,
	0b00010011,
	0b00010110,
	0b00011100,
	0b00010000,
	0b00011111,
	0b00000000
};
void lcd_custom_char (unsigned char loc, unsigned char *pattern)
{

	unsigned char i;
	if(loc<8)
	{
		lcd_wr_command(0x40 + (loc*8));
		for(i=0;i<8;i++)  
		lcd_wr_char(0,0,pattern[i]);
	}
}


void Display_logo()
{
	lcd_string(1,3,"Display Logo");
	_delay_ms(2000);
	lcd_clear();						// Clear the LCD
		lcd_custom_char(0, pattern_e);
		lcd_wr_char(2,8,0x00);
	
}
	

void Display_sine_wave()
{
	lcd_string(1,3,"Sine Wave");
	_delay_ms(2000);
	lcd_clear();						// Clear the LCD
	unsigned char character1[8] = {0x03,0x04,0x08,0x10,0x00,0x00,0x00,0x00};
	unsigned char character2[8] = {0x18,0x04,0x02,0x01,0x00,0x00,0x00,0x00};
	unsigned char character3[8] = {0x00,0x00,0x00,0x00,0x10,0x08,0x07,0x00};
	unsigned char character4[8] = {0x00,0x00,0x00,0x00,0x01,0x02,0x1C,0x00};
	lcd_custom_char(1,character1);
	lcd_custom_char(2,character2);
	lcd_custom_char(3,character3);
	lcd_custom_char(4,character4);
	unsigned char i,j;
	for(j=1,i=1;i<17;j++,i++){
		if(j==5)
		j=1;
		lcd_wr_char(1,i,j);
		lcd_wr_char(2,i,j);
	}


}

 int main(void)
{
	lcd_port_config();					// Initialize the LCD port
	lcd_init();							// Initialize the LCD
	lcd_clear();						// Clear the LCD
    
	Display_logo();
	_delay_ms(2000);
	lcd_clear();						// Clear the LCD
	Display_sine_wave();
	_delay_ms(2000);
	lcd_clear();						// Clear the LCD
	
    while (1);
}