


#include <p18cxxx.h>
#include <adc.h>        /*Premade functions for ADC*/
#include "InfraDead.h"


/* definitions used by IR_module */
#define IR_READINGS 0x04            //number of readings taken before calculating average, GLOBAL VARIABLE
#define AD_G0       ADCON0bits.GO

/* Global IR Variables */
signed int reading_mV;              //stores ADC reading in mV (*approx*)
signed int GLOBAL_MAX_mV = 3100;
signed int GLOBAL_MIN_mV = 200;






    
    
//int IR_DIST1;                        //stores output distance value (mm)
//int IR_DIST2;
//int IR_DIST3;


// Takes 4 readings from ADC, averages them and then calculates distance
int IRget(unsigned char IRchoose) //Read ADC 4 times, storing in reading buffer
{
    int i;
    int IR_result;              // for storing ADC reading result
    int reading_ave;
    int reading_sum = 0;
    signed int IR_Dist;         //holds actual distance output in 10^-4 m
    int IR_DIST;                //holds distance value in mm

    if (IRchoose == 1)
    {
        //set AD channel to RA1
        ADCON0bits.CHS0 = 1;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS2 = 0;
    }
    else if (IRchoose == 2)
    {      
        //set AD channel to RA0
        ADCON0bits.CHS0 = 0;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS2 = 0;
    }
    else if (IRchoose == 3)
    {
        //set AD channel to RA4
        ADCON0bits.CHS0 = 0;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS2 = 1;
    }

    for(i = 0;i < IR_READINGS; i++)
    {
        ADCON0bits.GO = 1;                  //Start conversion

        while( ADCON0bits.GO == 1 );             //Wait for conversion to finish
        IR_result = ReadADC();
        IR_result = IR_result>>8;   //Shift
        reading_sum = reading_sum + IR_result;
    }

    reading_ave = reading_sum/IR_READINGS;      //Average of sampled
    reading_mV = 20*reading_ave;

    if (reading_mV < GLOBAL_MAX_mV){ //&& reading_mV > GLOBAL_MIN_mV
        //Linearised voltage/distance plot
        if (reading_ave > 2000) IR_Dist = -3*reading_mV + 11400;
        else if (reading_mV > 1600) IR_Dist = -5*reading_mV + 16000;
        else if (reading_mV > 1250) IR_Dist = -6*reading_mV + 18000;
        else if (reading_mV > 1050) IR_Dist = -10*reading_mV + 23000;
        else if (reading_mV > 800) IR_Dist = -16*reading_mV + 28800;
        else if (reading_mV > 500) IR_Dist = -30*reading_mV + 42000;
        else IR_Dist = -75*reading_mV + 75000;
    }

    
    IR_DIST = IR_Dist/100; //converts to mm
    return IR_DIST;
    /*if (IRchoose == 1){
        IR_DIST1 = IR_DIST;
        return IR_DIST1;
    }
    else if (IRchoose == 2){
        IR_DIST2 = IR_DIST;
        return IR_DIST2;
    }
    else if (IRchoose == 3){
        IR_DIST3 = IR_DIST;
        return IR_DIST3;
    }*/
    
}


/*
void IRinitialise(void){
    
    volatile unsigned char START_IR =0;             //Flag to START IR sensor
    volatile unsigned char IRrateCURRENT=IRrate;    //Gets changed to PRESCALE IR RATE
    volatile unsigned char IRcurrent =0;            //Var to hold current IR sensor value


}*/