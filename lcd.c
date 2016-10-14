/*Include Files*/
#include <p18f4520.h>
#include "ConfigRegs18f4520.h"

#include <stdio.h>     
#include <stdlib.h>
#include <delays.h>

#define E_PIN	PORTDbits.RD6
#define RS_PIN	PORTDbits.RD4
#define RW_PIN	PORTDbits.RD5
#define UPPER_LINE 	0b10000000
#define LOWER_LINE 	0b11000000

void command(unsigned char comm){
	   
	PORTD = comm>>4;
    
	E_PIN = 1;
	E_PIN = 0;
    
	
	PORTD = comm & 0x0F;

    
	E_PIN = 1;
	E_PIN = 0;
	
	Delay1KTCYx(10);
}

void LCD_initialisation(void){
   
    TRISD = 0x00;
    
	E_PIN = 0;
	RS_PIN = 0;
	RW_PIN = 0;
	
	command(0x02);      	// return home
	command(0b00101000);	// two display lines
	command(0b00001100);	// entire display on, cursor on, cursor blink
	command(0b00000001);	// clear display
	command(0b00000010);	// return home

}

void print(unsigned char character){
	
	PORTD = character >> 4;
	RS_PIN = 1;
	E_PIN = 1;
	E_PIN = 0;
	RS_PIN = 0;
	
	PORTD = character & 0x0F;
	RS_PIN = 1;
	E_PIN = 1;
	E_PIN = 0;
	RS_PIN = 0;

	Delay1KTCYx(50);

}

void disp_line(unsigned char* printarray, unsigned char linepos, unsigned char replace){

	unsigned char counter = 0;
	
	if(replace == 1){
		
		command(linepos);
	
	} 
	
	while(*(printarray+counter) != '\0')
	{
		print(*(printarray+counter));
		counter++;
	}

}