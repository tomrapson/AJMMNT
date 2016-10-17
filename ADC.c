/* 
 * File:   ADC.c
 * Author: dphu8178
 *
 * Created on 12 October 2016, 12:46 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p18f4520.h>
#include "SerialStuff.h"
#include "ADC.h"

void ADCsetup(void)
{
    ADCON0bits.ADON = 1;
    return;
}


void getRSSI(void)
{
	//select pin 5 to read from
     ADCON0bits.CHS3 = 0;
     ADCON0bits.CHS2 = 1;
     ADCON0bits.CHS1 = 0;   
     ADCON0bits.CHS0 = 1;
     
     ADCON0bits.GO = 1; //start AD conversion
     while(!PIR1bits.ADIF){}    //wait for A/D
      
     RSSI = ADRESH;
     
     OUTGOINGdata.DataID = 'r';
     OUTGOINGdata.DataValue = RSSI;
     
     
     PIR1bits.ADIF = 0;     //reset ADIF
     
     PIE1bits.TXIE = 1;     // enable serial transmit
     return;
    
}

