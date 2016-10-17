/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PID_FUNCTIONS_H
#define PID_FUNCTIONS_H

#include <p18f4520.h> // include processor files - each processor file is guarded. 

#define TMR0_CON        0b10101000  //enable, 16bit, ext. pin, low-to-high, no prescale   
#define TMR1_CON        0b00000011  // 2 x 8-bit, external clock, 0 prescale, osc OFF, no SYNC, external clock, enable

#define	ERRORmax        0x0FA0	//setting my MAX ERROR (4000d)
#define ERRORaverage    0x0A

#define KI_INIT         0   //nothing yet                    
#define KD_INIT         0.1//0.07// 15
#define KP_INIT         0.262 // 0.262 looks hot, 0E were the best!!


//Defining the struct that stores all of my PID data for both wheels
struct PIDdata
{
    unsigned char   SPEEDset;     //set point decided by user
    unsigned char   SPEEDcurrent; //current speed from encoders
    unsigned char   Direction;    //direction of wheels
    unsigned char   ERRORcount;   //Count
    signed int      PIDoutput;     //current PWM output from PID (signed int as it interfaces between our signed/unsigned chars)
    signed int      ERRORcurrent; //difference b/w set and current
    signed int      ERRORlast;    //last value of ERRORcurrent
    signed int      ERRORaccum;   //accumulated error (for the Intergral)
    signed int      ERRORdiff;    //Difference between last and current
    signed int      ERRORrunAv; 
    
        
};

//Defining the struct that stores my gains for wheels L/R
//Will be set to the different factory values SOMEWHERE ELSE
struct PIDgains
{
    float Kp;       //proportional gain
    float Ki;       //intergral gain
    float Kd;       //derivate gain
        
};


//FUNCTION DECLERATIONS
extern volatile unsigned char ERRORflag;            //Flag to see if we have error
extern volatile unsigned char ERRORcount;           //variable to delay PID calcs by a factor of 10
extern volatile struct PIDdata LEFTwheel;           //L & R wheel values (GLOBAL)
extern volatile struct PIDdata RIGHTwheel;

extern volatile struct PIDgains PIDgainsLEFT;       //L & R PID gains (GLOBAL)   
extern volatile struct PIDgains PIDgainsRIGHT;

//PID functions
void PIDinitialise(void);               //Initialises my PID vars
void PIDerror(struct PIDdata *myPID);   //Calculates error b/w set speed and current speed
void PIDoutputPWM(struct PIDdata *myPID, struct PIDgains PIDgains );   //OUTPUTS TO PWM: Takes my gains and error

//Encoder functions
void ENCODERinitialise(void);
void ENCODERcapture(void);


#endif	/* XC_HEADER_TEMPLATE_H */