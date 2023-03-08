
#include "firebird_simulation.h"		// Header file included that contains macro definitions essential for Firebird V robot
#include <util/delay.h>					// Standard AVR Delay Library
#include "lcd.h"						// LCD Header file included that contains function definitions essential to deal with LCD
#include <stdio.h>

void lcd_print_wireframe(void)
{	
	lcd_string(1,1,"H:   D:    O:   ");
	lcd_string(2,1,"Binary: ");
	
}

void lcd_print_binary(unsigned char row_num, unsigned char col_num, unsigned char num)
{
    char buffer[9];
	int k=0;
	for(int c=8;c>=0;c--)
	 {
		 k=num>>(c-1);
		 if(k&1)
		  buffer[8-c] = '1';
		 else
		  buffer[8-c] ='0';
	 }
	
	lcd_string(row_num,col_num,buffer);
}

void lcd_print_octal(unsigned char row_num, unsigned char col_num, unsigned char num)
{
	   char buffer[4];
	   sprintf(buffer,"%.3o",num);
	   lcd_string(row_num,col_num,buffer);
}

void lcd_print_decimal(unsigned char row_num, unsigned char col_num, unsigned char num)
{
      char buffer[4];
      sprintf(buffer,"%.3d",num);
      lcd_string(row_num,col_num,buffer);
}

void lcd_print_hexadecimal(unsigned char row_num, unsigned char col_num, unsigned char num)
{
		char buffer[3];
		sprintf(buffer,"%.2X",num);
		lcd_string(row_num,col_num,buffer);
}

int main(void){
	
	
	lcd_port_config();					// Initialize the LCD port
	lcd_init();							// Initialize the LCD
	lcd_clear();						// Clear the LCD
	lcd_print_wireframe();				// Print wireframe on the LCD
	unsigned char counter = 0;

	while(1){
		lcd_print_hexadecimal(1,3,counter );	// Pass the relevant arguments to print hexadecimal characters on the LCD at the place specified by the wireframe
		lcd_print_decimal(1,8,counter);		// Pass the relevant arguments to print decimal characters on the LCD at the place specified by the wireframe
		lcd_print_octal(1,14,counter );			// Pass the relevant arguments to print octal characters on the LCD at the place specified by the wireframe
		lcd_print_binary( 2,9,counter );		// Pass the relevant arguments to print binary characters on the LCD at the place specified by the wireframe
		counter++;	// Increment the counter
		_delay_ms(500);
	}
}
