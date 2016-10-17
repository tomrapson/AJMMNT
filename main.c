
/*Include Files*/
#include <p18f4520.h>
#include "ConfigRegs18f4520.h"
#include "main.h"
#include "IoDefs.h"
#include "PIDFunctions.h"
#include "MotorInputDefs.h"
#include "InfraDead.h"
#include "ADC.h"
#include "XbeeSerial.h"
#include "SerialStuff.h"
#include <stdio.h>     
#include <stdlib.h>

//Main Variables (mostly flags for main))

unsigned char direction = FORWARD;  //TAKE THIS OUT LATER
volatile unsigned char START_IR =0;             //Flag to START IR sensor
volatile unsigned char IRrateCURRENT=0;    //Gets changed to PRESCALE IR RATE
volatile unsigned char IRcurrent =0;            //Var to hold current IR sensor value
void move(void);
  

//INTERUPT SERVICE ROUTINES
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


/*<><><><><><><><><><><><><><>><><><><><><><><><><><><><><
//<><>><><><><><><>MAIN LOOP<><><><><>><>><><><><><><><><>
//<><><><><><><><><><><><><><>><><><><><><><><><><><><><>*/
#pragma code
void main(void)
{   
       
    setup();
    
    //FOR TESTIN
    RIGHTspeed = 0;
    LEFTspeed = 0;
           
    //MAIN
    while(1)
    { 
            
        
        move();
        
        
        
        //Checking IR sensors
        if (START_IR==0){
         //   IRget(IRcurrent);   //where IRcurrent specifies which IR sensor we want
        }
    }
}


/*<><><><><><><><><><><><><><>><><><><><><><><><><><><><><
//<><>><><><><><><>INTERRUPTS<><><><><>><>><><><><><><><><
//<><><><><><><><><><><><><><>><><><><><><><><><><><><><>*/
#pragma interrupt highPriorityIsr
void highPriorityIsr()
{    
    //IR sensors interrupt
    if(PIR2bits.TMR3IF==1)  
    {
        ENCODERcapture();
        PIR2bits.TMR3IF =0; //RESET
        return;
        
        IRrateCURRENT--;            //decrement the prescale counter
        if (IRrateCURRENT==0)       //when ==0, take IR sensors
        {
            START_IR=1;             //Flag to start IR readings
            IRrateCURRENT=IRrate;   //Reset the prescale counter
        }
    }
    //recieve interrupt 
    else if (PIR1bits.RCIF)
    {
        rx232Isr();
        return;
    }
    
    //transmit interrupt
    else if(PIR1bits.TXIF)
    {
        
        tx232Isr();
        return;
    }    
}

#pragma interruptlow lowPriorityIsr
void lowPriorityIsr()
{
    //Send stuff over serial
    //Check serial buffer
    //Scan for parrot??
}


/*<><><><><><><><><><><><><><>><><><><><><><><><><><><><><
//<><>><><><><><><>SETUP<><><><><>><>><><><><><><><><><><>
//<><><><><><><><><><><><><><>><><><><><><><><><><><><><>*/
void setup(void)
{    
    
    //IRinitialise();
    IRrateCURRENT=IRrate; 
    
    ISRturnOFF();           //Turn them off to be safe

    ISRinitialise();        //Sets ISR preferences
    
    ConfigIO();             //CONFIGURES ALL PIN I/O
       
    ENCODERinitialise();    //Configures TMR 0 and 1
    
    TMR3initialise();       //Configurs TMR3
    
    PIDinitialise();        //configures values for wheel speeds and PID error
             
    ConfigPWM();            //Configure TMR2, turns on CCP modules 1 & 2
    
    setSerial();            //serial setup
     
    ADCsetup();
    
    ISRturnON();            //Turns them on so we can roll
    
}


void ISRinitialise(void){
    
    //EXTERNAL INTERRUPT CONFIGS AND SETUPS
    INTCON3bits.INT2IP = 1;
    INTCON3bits.INT2IE = 1;

    //assign priorities, most things are going to be low
    IPR1bits.RCIP = 0;  //low priority
    
    //Allow interrupt priority
    RCONbits.IPEN = 1;
}

void ISRturnOFF(void){
    INTCON = 0;
    PIR1 = 0;
}


void ISRturnON(void){
    
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
}

void TMR3initialise (void){
    
    PIR2bits.TMR3IF=0;  //set to 38Hz
    PIE2bits.TMR3IE=1;
    T3CON = TMR3_CON;
}

void move(void){
    
    //Takes values from 0-255 negative MAX speed to positive MAX speed
        //and output magnitude for encoders, with direction
        SCALEspeed(RIGHTspeed,RIGHTWHEEL,&RIGHTwheel);
        SCALEspeed(LEFTspeed,LEFTWHEEL,&LEFTwheel);
        
        //PID calculation stuff (FINDS ERROR)
        PIDerror(&LEFTwheel);
        PIDerror(&RIGHTwheel);
        
        //IF ERROR (if there is a difference between set and current, change motor speed)
        if (ERRORflag==1){
            
            //Calculates change in PWM based off error
            PIDoutputPWM(&LEFTwheel, PIDgainsLEFT );//Gets passed a pointer and a copy (copy for safeguard))
            PIDoutputPWM(&RIGHTwheel, PIDgainsRIGHT ); 
            //Sets direction
            direction = ( RIGHTwheel.Direction|LEFTwheel.Direction);
            SetDirection(direction); 
            //Sets PWM values
            SetSpeed(RIGHTwheel.PIDoutput, RIGHTWHEEL); //Left Motor
            SetSpeed(LEFTwheel.PIDoutput, LEFTWHEEL); //Right Motor
        }    
}