/* 
 * File:   lcd.h
 * Author: Thomas Rapson & Meg Flannery
 *
 * Created on 14 October 2016, 15:39
 */

#ifndef LCD_H
#define	LCD_H
	void LCD_initialisation(void);
	void disp_line(unsigned char* printarray, unsigned char linepos, unsigned char replace);
#endif	/* LCD_H */