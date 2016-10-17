/**
 * @file main.h
 * @author Team Elizabeth
 * @date 5th october 2016
 * @brief Header file for all my vars
 * /



/*Include Files*/
#ifndef MAIN_H
#define MAIN_H

#include <p18f4520.h>
#include "ConfigRegs18f4520.h"
#include "IoDefs.h"
#include "MotorInputDefs.h"
#include "SerialStuff.h"
#include <stdio.h>     
#include <stdlib.h>
#include <adc.h>


#define TMR3_CON 0b11000001;


extern volatile unsigned char START_IR;     //Flag to START IR sensor
extern volatile unsigned char IRcurrent;    //Current IR sensor we are checking
extern volatile unsigned char IRrateCURRENT;  


/*Define the interrupts*/
void low_interrupt( void );
void high_interrupt( void );
void lowPriorityIsr( void );
void highPriorityIsr( void );

//OTHER SETUP
void setup(void);
void ISRinitialise(void);
void ISRturnON(void);
void ISRturnOFF(void);
void TMR3initialise (void);






#endif /* MAIN_H*/