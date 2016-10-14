/* 
 * File:   lcd.h
 * Author: Thomas Rapson & Meg Flannery
 *
 * Created on 14 October 2016, 15:39
 */

#ifndef LCD_H
#define	LCD_H
#define UPPER_LINE  0
#define LOWER_LINE  1
	void LCD_init(void);
	void putcLCD(unsigned char);
    void putsLCD(unsigned char*,unsigned char);
#endif	/* LCD_H */