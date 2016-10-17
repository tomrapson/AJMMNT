/* 
 * File:   MotorInputDefs.h
 * Author: Tristan James Hall of House Gilpin
 *
 * Created on 8 October 2015, 4:17 PM
 */

#ifndef MOTORINPUTDEFS_H
#define	MOTORINPUTDEFS_H

#include "PIDFunctions.h"   //inclusing so that it can see the struct type

/*Define all constants*/


#define PWM_SETTINGS    0b00001100  //CCP modules PWM mode
#define PWM_PERIOD      0x1000      //cycles for PWM freq
#define TMR2_CON        0b00000100



#define BACKWARD    0b00011101
#define FORWARD     0b00101011

#define FORR_FORL   0b00101011
#define BACKR_BACKL 0b00011101
#define FORR_BACKL  0b00011011
#define BACKR_FORL  0b00101101

#define FORR    0b00000011
#define FORL    0b00101000
#define BACKR   0b00000101
#define BACKL   0b00011000

#define BREAK   0b00000000
#define BREAK2  0x00111111

#define ISFORWARD 0
#define ISBACKWARD 1

#define LEFTWHEEL 0     //Boolean identified for left wheel
#define RIGHTWHEEL 1    //Boolean identifier for right wheel



/*Declare functions*/
void SetDirection(unsigned char);
void ConfigPWM(void);
void SetSpeed(unsigned char, unsigned char);
void SCALEspeed(unsigned char SPEEDin, unsigned char motorsel, struct PIDdata *WHEELdata);

#endif	/* MOTORINPUTDEFS_H */

