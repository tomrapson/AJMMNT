/////////////////////////////////////////
//		Question 7		//
//////////////////////////////////////////

// Task: Interrupt based serial transmit and recieve
// Send a string to the SCI
// Same as Question 7
// Author: Team Elizabeth

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <p18f4520.h>
#include "ConfigRegs_18F4520.h"
#include <usart.h>  // microchip's C18 USART library
#include <delays.h>
#include "SerialStuff.h"

//interrupt setup
#pragma code rx232Isr = 0x08
#pragma code tx232Isr = 0x18

//allocate memory for the structs
volatile DataStruct INCOMINGdata;
volatile DataStruct OUTGOINGdata;

//define functions
void setSerial(void);


#pragma interrupt tx232Isr
void tx232Isr(void) {

   PIE1bits.TXIE = 0;
   
   while(BusyUSART()){};//wait till now busy
   putcUSART(OUTGOINGdata.DataID);//send first dataID character
   
   //special case if the data is 'e' (unlucky)
   if(OUTGOINGdata.DataValue == 'e')
   {
       while(BusyUSART()){};//wait till now busy
       putcUSART('\b');//first send \r so we know the e is genuine data
      
    }
   else
   {
       while(BusyUSART()){};//wait till now busy
       putcUSART(OUTGOINGdata.DataValue);//first send \r so we know the e is genuine data     
       
   } 
   
   PIE1bits.TXIE = 1;
   return;
}

#pragma interrupt rx232Isr
void rx232Isr(void) {
    
    PIE1bits.RCIE = 0;//turn off receive while we are processing current data
    
    INCOMINGdata.DataID = getcUSART();
    
    if(INCOMINGdata.DataID == 'e')
    {
        //get RSSI
        return;
    }
    
    while(!DataRdyUSART()){}    //wait till data ready
    
    INCOMINGdata.DataValue = getcUSART();   //get data value
    
    if (INCOMINGdata.DataValue == '\b')     //check if the data value is 'e'
    {
        INCOMINGdata.DataValue = 'e';       
    }   
    else if(INCOMINGdata.DataID == 'e')
    {
        
        //get RSSI
        
        return;
    }
 
    PIE1bits.RCIE = 1;      //re-enable receive interrupt
    return;   
}

void main(void)
{   
    //initialise some bogus data
    INCOMINGdata.DataID = 'a';
    INCOMINGdata.DataValue = 'b';
    
    OUTGOINGdata.DataID = 'c';
    OUTGOINGdata.DataValue = 'e'; 
    
    setSerial(); 
    
    for (;;) {
    }
}


/////////////////////////////////////////////////////////////////////////////
//                   Serial Setup                                          //
////////////////////////////////////////////////////////////////////////////
void setSerial(void)
{
   
   TRISCbits.TRISC6 = 0;    //set port c pin 6 as output 
   TRISCbits.TRISC7 = 1;    //set port c pin 7 as input   
   PIE1bits.TXIE = 1;       //enable transfer interrupt
   PIE1bits.RCIE = 1;       //enable receive interrupt
   
   //USART setup
   OpenUSART(   USART_TX_INT_ON &
                USART_RX_INT_ON    &
                USART_ASYNCH_MODE   &
                USART_EIGHT_BIT &
                USART_CONT_RX   &
                USART_BRGH_HIGH, 64);
   
   RCONbits.IPEN = 1;           //enable priority levels
   INTCONbits.PEIE_GIEL = 1;    //enable low priority interrupts
   INTCONbits.GIEH = 1;         //global interrupt enable
   TXSTAbits.TXEN=1;            //enable serial transmit
   
   return;
  
}