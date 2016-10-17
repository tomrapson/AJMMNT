

#include <p18f4520.h>
#include "PIDFunctions.h"







volatile unsigned char ERRORflag = 0;   //Error flag

volatile struct PIDdata     LEFTwheel;        
volatile struct PIDdata     RIGHTwheel;
volatile struct PIDgains    PIDgainsLEFT;        
volatile struct PIDgains    PIDgainsRIGHT;


//Takes set speed and current speed, calculates error values
void PIDerror(struct PIDdata *myPID)
{
    
    
    myPID->ERRORcurrent = (int) myPID->SPEEDset  -  myPID->SPEEDcurrent; //finding current error
    
    myPID->ERRORrunAv = myPID->ERRORrunAv + myPID->ERRORcurrent - myPID->ERRORrunAv/myPID->ERRORcount; //calcuating moving average
    
         
    // Our 10 averaged counts are ready to fire
    if (myPID->ERRORcount >= ERRORaverage ){
            
        
        myPID->ERRORcurrent = myPID->ERRORrunAv/myPID->ERRORcount;
                
        //This saves going into the next function to recalcualte PID value
        if (myPID->ERRORcurrent == 0)
        {             
            ERRORflag = 0;              // set error zero flag CAN BE USED TO MAKE CODE LESS INTENSIVE
        }
        else
        {
            ERRORflag = 1;
        }
         
        myPID->ERRORdiff = myPID->ERRORcurrent - myPID->ERRORlast;  // calc delta error
        myPID->ERRORlast = myPID->ERRORcurrent;      // set previous error to current error for next round
        
        myPID->ERRORaccum+=myPID->ERRORcurrent; //accumulating error
        if (myPID->ERRORaccum > ERRORmax ) //Is checking error within bounds, sets to max error
        {        // calculate accumulated error
            myPID->ERRORaccum = ERRORmax ;
        }
        
        myPID->ERRORcount = 1;    //reset error count
    }
    else{
        
    myPID->ERRORcount++;
    
    }
    
 }


//Takes error values and calculates new PWM
void  PIDoutputPWM(struct PIDdata *pwmPID, struct PIDgains PIDgains )
{
    signed int propTEMP;    //proportional error
    signed int interTEMP;   //intergral error
    signed int derivTEMP;   //derivaite error

    propTEMP =  PIDgains.Kp * pwmPID->ERRORcurrent  ;
    interTEMP = PIDgains.Ki * pwmPID->ERRORaccum ;
    derivTEMP = PIDgains.Kd * pwmPID->ERRORdiff  ;
        
    //Setting our new PWM output
    pwmPID->PIDoutput += propTEMP+interTEMP+derivTEMP;
    
    if (PIDgains.Kp ==0 && PIDgains.Ki ==0 && PIDgains.Kd ==0){
        
        pwmPID->PIDoutput=pwmPID->SPEEDset;
        
    }
    
    if (pwmPID->SPEEDset==0){
        pwmPID->PIDoutput=0;
    }
        
    //checking bounds for the PWMoutput
    if((pwmPID->PIDoutput) > 0xFF)  
    {
        pwmPID->PIDoutput=0xFF;
    }
    else if((pwmPID->PIDoutput) < 0)
    {
        pwmPID->PIDoutput=0;
            
    }

}




/**
 * @brief This reads the encoder counts
 */
void ENCODERcapture(void) { //Reads the encoder wheel speed
    unsigned int LEFTencoder =  (TMR0H << 8) + TMR0L; //Temporary var which holds speed
    unsigned int RIGHTencoder = (TMR1H << 8) + TMR1L;
    TMR0H = 0;
    TMR0L = 0;
    TMR1H = 0;
    TMR1L = 0;

    if (RIGHTencoder > 85) //Prevents overflow
    {
        RIGHTencoder = 85;
    }
    if (LEFTencoder > 85) {
        LEFTencoder = 85;
    }
    
    RIGHTencoder =  RIGHTencoder    *   3;
    LEFTencoder  =  LEFTencoder     *   3;
   
    RIGHTwheel.SPEEDcurrent = (unsigned char) RIGHTencoder;
    LEFTwheel.SPEEDcurrent = (unsigned char) LEFTencoder ;
    
}





void ENCODERinitialise(void){
    
    T0CON = TMR0_CON; //Timer On, 16 bit, Externally Clocked (RA4), BYPASS Prescaler
    T1CON = TMR1_CON;  // 2 8-bit reads, zero prescale, oscilator disabled, DO NOT sync, external clock (PC0), TMR enabled
    

    PIR1bits.TMR1IF=0;
    INTCON3bits.INT1E=0;
    //T0CON = 0b10000011;
    
}



void PIDinitialise(void)
{
    
    //Initialising PID vars for LEFT and RIGHT wheels
    LEFTwheel.ERRORaccum=0;
    LEFTwheel.ERRORcurrent=0;
    LEFTwheel.ERRORdiff=0;
    LEFTwheel.ERRORlast=0;
    LEFTwheel.PIDoutput=0;
    LEFTwheel.SPEEDcurrent=0;
    LEFTwheel.SPEEDset=0;
    LEFTwheel.Direction=0;
    LEFTwheel.ERRORcount = 1; //delay reading of PID by factor of 10
    LEFTwheel.ERRORrunAv=0;
    
    RIGHTwheel.ERRORaccum=0;
    RIGHTwheel.ERRORcurrent=0;
    RIGHTwheel.ERRORdiff=0;
    RIGHTwheel.ERRORlast=0;
    RIGHTwheel.PIDoutput=0;
    RIGHTwheel.SPEEDcurrent=0;
    RIGHTwheel.SPEEDset=0;
    RIGHTwheel.Direction = 0;
    RIGHTwheel.ERRORcount = 1;
    RIGHTwheel.ERRORrunAv=0;
    
    //Initialsing Gains for LEFT and RIGHT wheels
    PIDgainsLEFT.Kd  =  KD_INIT;
    PIDgainsLEFT.Ki  =  KI_INIT;
    PIDgainsLEFT.Kp  =  KP_INIT;
    
    PIDgainsRIGHT.Kd =  KD_INIT;
    PIDgainsRIGHT.Ki =  KI_INIT;
    PIDgainsRIGHT.Kp =  KP_INIT;     
    
    
    
}