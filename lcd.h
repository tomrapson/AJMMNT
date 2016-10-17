/* 
 * File:   lcd.h
 * Author: Thomas Rapson & Meg Flannery
 *
 * Created on 14 October 2016, 15:39
 */

#ifndef LCD_H
#define	LCD_H
#define MAX_LCD_CHARS 16
#define LINE1  0x0F
#define LINE2  0xFF
#define CLR_DISP    0b00000001
	void LCD_init(void);
	void cmd2LCD(unsigned char);
	void putcLCD(unsigned char);
    void putsLCD(unsigned char*,unsigned char);
#endif	/* LCD_H */