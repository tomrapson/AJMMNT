/* 
 * File:   lcd.c
 * Author: Thomas Rapson and Meg Flannery
 *
 * Created on 14 October 2016, 11:26 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <delays.h>
#include "p18f4520.h"
#include "ConfigRegs18f4520.h"

#define E_PIN   PORTDbits.RD6
#define RS_PIN  PORTDbits.RD4
#define RW_PIN  PORTDbits.RD5
#define BF  PORTDbits.RD7
#define CLR_DISP    0b00000001
#define RET_HOME    0b00000010
#define R2C1        0xC0
#define CURSOR_R    0x14
#define CURSOR_L    0x10
#define FUNCTION_SET    0b00101000
#define DISP_ON 0b00001100

void num2str(unsigned char *buf, unsigned char number){
    unsigned char i, remainder, length = 0, n;
 
    n = number;
	
	// find number of digits
    while (n != 0)
    {
        length++;
        n /= 10;
    }
	
	// store each digit as a character in the buffer
    for (i = 0; i < length; i++)
    {
        remainder = number % 10;
        number = number / 10;
        buf[length - (i + 1)] = remainder + '0';
    }
	
    buf[length] = '\0'; // append a null character
}

void cmd2LCD(unsigned char cmd){
	// write upper nibble   
	PORTD =  cmd >> 4;
	E_PIN = 1;
	E_PIN = 0;
    
    // write lower nibble
	PORTD = cmd & 0x0F;
	E_PIN = 1;
	E_PIN = 0;
	
	Delay100TCYx(15); // short delay to wait for busy flag to clear
}

void LCD_init(void){
   
    TRISD = 0x00; // PORTD output
    
	E_PIN = 0;
	RS_PIN = 0;
	RW_PIN = 0;
	
	cmd2LCD(FUNCTION_SET);	// two display lines
	cmd2LCD(DISP_ON);	// entire display on, cursor on, cursor blink
	cmd2LCD(CLR_DISP);	// clear display
	cmd2LCD(RET_HOME);	// return home
}

void putcLCD(unsigned char c){
	// write upper nibble
	PORTD = c >> 4;
	RS_PIN = 1;
	E_PIN = 1;
	E_PIN = 0;
	RS_PIN = 0;
	
    // write lower nibble
	PORTD = c & 0x0F;
	RS_PIN = 1;
	E_PIN = 1;
	E_PIN = 0;
	RS_PIN = 0;

	Delay100TCYx(40); // short delay to wait for busy flag to clear

}

void clrLine(void){
    unsigned char i;
    for(i = 0; i < 16; i++){
        putcLCD(' ');
    }
}

void putsLCD(unsigned char *ptr,unsigned char pos){
    switch(pos){
        // if the string is to be output to the upper line, return home
        case 0 :
            cmd2LCD(RET_HOME);
            clrLine();
            cmd2LCD(RET_HOME);
            break;
            
        // if the string is to be output to the lower line, go to start of lower line
        case 1 :
            cmd2LCD(R2C1);
            clrLine();
            cmd2LCD(R2C1);
            break;
            
        // if the string is to be output further along the current line, shift the cursor 'pos' times
        default :
            while(pos){
                cmd2LCD(CURSOR_R);
                pos--;
            }
            break;
    }
    
    while(*ptr){ // until the null character is reached
        putcLCD(*ptr); // write the character to the LCD
        ptr++; // go to the next character
    }
}