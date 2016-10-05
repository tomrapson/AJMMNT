/*Author: Tristan Hall from House Gilpin*/

/*Include Files*/
#include <p18f4520.h>
#include "ConfigRegs18f4520.h"
#include "main.h"
#include "IoDefs.h"
#include "MotorInputDefs.h"
#include "SerialStuff.h"
#include "AllMyVariables.h"
#include <stdio.h>     
#include <stdlib.h>
#include <adc.h>



//THIS IS THE MAIN LOOP
void main(void)
{   
       
    setup();
           
    //ALL WE ARE DOING IS TURNING ON THE PWMs
    while(1)
    { 
                   
        SENDdataID ='b';
        SENDserialPACKAGE();
        
            ConvertInput(rawinput1,THELEFT,&realinput1,&wheelsel1);
            ConvertInput(rawinput2,THERIGHT,&realinput2,&wheelsel2);
            
            direction = (wheelsel1|wheelsel2);

            SetDirection(direction); //Set direction

            SetSpeed(realinput1, LEFTMOTOR); //Left Motor
            SetSpeed(realinput2, RIGHTMOTOR); //Right Motor
    }
}





/*
//THE ISRs DONT DO ANYTHING AT THE MOMENT
#pragma code highPriorityInterruptAddress=0x0008
void high_interrupt()
{
    highPriorityIsr();
}

#pragma code lowPriorityInterruptAddress=0x0018
void low_interrupt()
{
    lowPriorityIsr();
}

#pragma code

#pragma interrupt highPriorityIsr
void highPriorityIsr()
{    
    GETserial(); //some dummy thing where I get serial and store it into a struct
    
}

#pragma interruptlow lowPriorityIsr
void lowPriorityIsr()
{
    ;
}



*/


void setup(void)
{
    
    //CONFIGURES ALL PIN I/O
    ConfigIO();
    
    //Config the PWM for Motors
    ConfigPWM();
    
    /*TURN OFF ALL INTERRUPTS TO BE SAFE*/
    INTCON = 0;
    PIR1 = 0;
    
    //peripheral interrupts
    PIE1bits.RCIE = 1;  //Going to be receiving
    INTCONbits.TMR0IE = 1; //Timer0 overflow    
    
    //EXTERNAL INTERRUPT CONFIGS AND SETUPS
    INTCON2bits.INTEDG1 = 1;    //Rising edge
    INTCON2bits.INTEDG2 = 1;
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT2IP = 1;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IE = 1;
    
    //assign priorities, most things are going to be low
    IPR1bits.RCIP = 0;  //low priority
    INTCON2bits.TMR0IP = 1; //high priority for handshake
    
    //Allow interrupt priority
    RCONbits.IPEN = 1;
    
        
    
    //Enable the timer in 16 bit
    T0CON = 0x88;   

    //Reactivate interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

}