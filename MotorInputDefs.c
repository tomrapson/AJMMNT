/**
 * @file AllMyVariables.h
 * @author Team Elizabeth
 * @date 5th october 2016
 * @brief This defines all of our variables
 * /
/*Required include files*/
#include <p18f4520.h>
#include "MotorInputDefs.h"



/*First argument is the value to be written to the output pins to determine direction*/
void SetDirection(unsigned char direction)
{
    PORTD = direction;  //Set the input direction
}

/*Configure the timer and period for the PWM, does not cover duty cycle*/
void ConfigPWM(void)
{
    T2CON =  0x05;  //enable timer
    PR2 = 0xff;     //max out the period
    CCP1CON = 0x0c; //Set both CCP modules to PWM
    CCP2CON = 0x0c;
}

/*Set the speed, given a metric based on the controller*/
/*INPUT 1 is the 8bit value corresponding to a speed*/
/*INPUT 2 is  a simple 0 or 1 bit deciding which motor is being changed*/
void SetSpeed(unsigned char speed, unsigned char motor)
{
    //Check if correct selection
    if( (motor == LEFTMOTOR)||(motor == RIGHTMOTOR) )
    {
        //Check which motor to set
        if ( motor == RIGHTMOTOR )
        {
            CCPR1L = speed; //Duty cycle of PWM
        }
        else if ( motor == LEFTMOTOR )
        {
            CCPR2L = speed; //Duty cycle of PWM
            CCPR1L = speed; //Duty cycle of PWM
        }
    }
    else
    {
        ;   //Shouldn't do anything, would prefer a compiler error though
    }
}

//Input the raw data and motor selection. Will need to determine this via some code form
void ConvertInput(unsigned char rawinput, unsigned char motorsel, unsigned char* realinput, unsigned char* wheelsel)
{
    int tempdir;
    
    /*First step is to tell if the input is in our desired range*/
    if(rawinput >= 128)
    {
        /*Subtract the value by 128 and just double it*/
        rawinput = rawinput - 128;
        rawinput = rawinput*2;
        *realinput = rawinput;
        tempdir = ISFORWARD; 
    }
    else if (rawinput < 127)
    {
        /*Take the raw input and minus 127 before doubling it*/
        rawinput = (127 - rawinput);
        rawinput = rawinput*2;
        *realinput = rawinput;
        tempdir = ISBACKWARD;
    }
    
    /*Next step is to determine if the wheel is a left or a right*/
    /*Change this condition later. LEFT WHEEL*/
    if(motorsel == 0)
    {
        /*Then understand wheel rotation*/
        if(tempdir == ISFORWARD)
        {
            *wheelsel = FORL;
        }
        else if (tempdir == ISBACKWARD)
        {
            *wheelsel = BACKL;
        }
    }
    /*RIGHT WHEEL*/
    else if (motorsel == 1)
    {
        if(tempdir == ISFORWARD)
        {
            *wheelsel = FORR;
        }
        else if (tempdir == ISBACKWARD)
        {
            *wheelsel = BACKR;
        }
    }
}