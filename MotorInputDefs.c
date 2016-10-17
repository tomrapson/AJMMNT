/**
 * @file AllMyVariables.h
 * @author Team Elizabeth
 * @date 5th october 2016
 * @brief This defines all of our variables
 * /
/*Required include files*/
#include <p18f4520.h>
#include "MotorInputDefs.h"
#include "PIDFunctions.h"



/*First argument is the value to be written to the output pins to determine direction*/
void SetDirection(unsigned char direction)
{
    PORTD = direction;  //Set the input direction
}

/*Configure the timer and period for the PWM, does not cover duty cycle*/
void ConfigPWM(void)
{
    T2CON =  TMR2_CON;  //enable timer
    CCP1CON = PWM_SETTINGS; //Set both CCP modules to PWM
    CCP2CON = PWM_SETTINGS;
    PR2 = 0xff;     //max out the period
}

/*Set the speed, given a metric based on the controller*/
/*INPUT 1 is the 8bit value corresponding to a speed*/
/*INPUT 2 is  a simple 0 or 1 bit deciding which motor is being changed*/
void SetSpeed(unsigned char SetSpeed, unsigned char motor)
{
    //Check if correct selection
    if( (motor == LEFTWHEEL)||(motor == RIGHTWHEEL) )
    {
        //Check which motor to set
        if ( motor == RIGHTWHEEL )
        {
            CCPR2L = SetSpeed; //Right wheel speed
        }
        else if ( motor == LEFTWHEEL )
        {
            CCPR1L = SetSpeed; //Left Wheel Speed
        }
    }
    else
    {
        ;   //Shouldn't do anything, would prefer a compiler error though
    }
}

//Input the raw data and motor selection. Will need to determine this via some code form
void SCALEspeed(unsigned char SPEEDin, unsigned char motorsel, struct PIDdata *WHEELdata)
{
    
    int tempdir;
    
    /*First step is to tell if the input is in our desired range*/
    if(SPEEDin >= 128)
    {
        /*Subtract the value by 128 and just double it*/
        SPEEDin = SPEEDin - 128;
        SPEEDin = SPEEDin*2;
        WHEELdata->SPEEDset = SPEEDin;
        WHEELdata->Direction = ISFORWARD;
        tempdir = ISFORWARD; 
    }
    else if (SPEEDin < 127)
    {
        /*Take the raw input and minus 127 before doubling it*/
        SPEEDin = (127 - SPEEDin);
        SPEEDin = SPEEDin*2;
        WHEELdata->SPEEDset = SPEEDin;
        tempdir = ISBACKWARD;
    }
    
    /*Next step is to determine if the wheel is a left or a right*/
    /*Change this condition later. LEFT WHEEL*/
    if(motorsel == 0)
    {
        /*Then understand wheel rotation*/
        if(tempdir == ISFORWARD)
        {
            WHEELdata->Direction =  FORL;
        }
        else if (tempdir == ISBACKWARD)
        {
            WHEELdata->Direction = BACKL;
        }
    }
    /*RIGHT WHEEL*/
    else if (motorsel == 1)
    {
        if(tempdir == ISFORWARD)
        {
            WHEELdata->Direction = FORR;
        }
        else if (tempdir == ISBACKWARD)
        {
            WHEELdata->Direction =  BACKR;
        }
    }
}