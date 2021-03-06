// lcd.c
//
// Keenan Johnson
// Franco Santa-Maria
//
// CpE 4160
// FALL 2014

#include "lcd.h"

void lcd_write_cmd( uint8 cmd )
{
    P0 = cmd;
	RS = 0;
	E = 1;
	delay(5);
    E = 0;
    P0 = 0xFF;
}

void lcd_write_data( uint8 dat )
{
    P0 = dat;
	RS = 1;
	E = 1; 
	delay(5);
    E = 0;
    P0 = 0xFF;
}

void lcd_init()
{
	lcd_write_cmd( 0x38 );
	delay( 50 );

	lcd_write_cmd( 0x0C );
	delay( 50 );

	lcd_write_cmd( 0x06 );
	delay( 50 );

	// Sends the clear command
	lcd_write_cmd( 0x01 );
	delay( 50 );
}

void lcd_print_array( uint8 *array )
{
    uint8 *iterator;
    
    // Get start of array
    iterator = array;
    
    // Check for nul terminator
    while ( *iterator != NUL )
    {
        // Write data at that location
        lcd_write_data( *iterator );
        
        // Increment the iterator
        iterator++;
    }
}

void lcd_clear()
{
	lcd_write_cmd( 0x01 );
	delay( 10 );
}

void lcd_print_led_status( uint8 right_left, uint16 led_delay )
{
	char delay_string[16];

	// Clear  Screen
	lcd_clear();

    // Write Direction
	if ( right_left == 0 )
	{
		lcd_print_array( "Direction: Left");
	}
	else 
	{
		lcd_print_array( "Direction: Right");	
	}

	// Changes to second line
	lcd_write_cmd( 0xC0 );

	// Convert delay to string
	sprintf( delay_string, "Delay: %u ms", led_delay );

	// Print Delay
	lcd_print_array( delay_string);
}
