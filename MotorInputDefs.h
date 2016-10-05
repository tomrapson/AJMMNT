/* 
 * File:   MotorInputDefs.h
 * Author: Tristan James Hall of House Gilpin
 *
 * Created on 8 October 2015, 4:17 PM
 */

#ifndef MOTORINPUTDEFS_H
#define	MOTORINPUTDEFS_H

/*Define all constants*/
#define BACKWARD 0b00011101
#define FORWARD 0b00101011

#define FORR_FORL 0b00101011
#define BACKR_BACKL 0b00011101
#define FORR_BACKL 0b00011011
#define BACKR_FORL 0b00101101

#define FORR 0b00000011
#define FORL 0b00101000
#define BACKR 0b00000101
#define BACKL 0b00011000

#define BREAK 0b00000000
#define BREAK2 0x00111111
#define RIGHTMOTOR 0
#define LEFTMOTOR 1

#define ISFORWARD 0
#define ISBACKWARD 1


#define THELEFT 0
#define THERIGHT 1

/*Declare functions*/
//void ConfigIO(void);
void SetDirection(unsigned char);
void ConfigPWM(void);
void SetSpeed(unsigned char, unsigned char);
void ConvertInput(unsigned char, unsigned char, unsigned char*, unsigned char*);

#endif	/* MOTORINPUTDEFS_H */

