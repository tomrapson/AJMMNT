///////////////////////////////////////////
 //		Question 7		                 //
 //////////////////////////////////////////
 
 // Description: receive and transmit ISR's and related functions for serial 
 // communication between the the Xbees. This includes handling of RSSI requests
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
 #include "XbeeSerial.h"
// #include "ADC.h"
 
 volatile DataStruct INCOMINGdata;        
 volatile DataStruct OUTGOINGdata;
 
 int RSSI = 0;   

void main(void)
{
    INCOMINGdata.DataValue = 'q';
    INCOMINGdata.DataID = 'd';
    
    OUTGOINGdata.DataValue = 'w';
    OUTGOINGdata.DataID = 'z';
    
 setSerial()  ;
 ADCsetup();

 for(;;){}
    

}



 /////////////////////////////////////////////////////////////////////////////
 //                   Serial Setup                                          //
 ////////////////////////////////////////////////////////////////////////////
 void setSerial(void)
 {
    
    TRISCbits.TRISC6 = 1;    //set port c pin 6 as output 
    TRISCbits.TRISC7 = 1;    //set port c pin 7 as input  
    
    PIE1bits.TXIE = 1;       //enable transfer interrupt
    PIE1bits.RCIE = 1;       //enable receive interrupt
    
    TXSTAbits.TXEN = 1;
    TXSTAbits.BRGH = 1;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    
    RCONbits.IPEN = 1;
  
    INTCONbits.PEIE =1; 
    BAUDCONbits.BRG16 = 0;
    SPBRG = 64;             //9600 baud rate
    
    INTCONbits.GIEH = 1;
    
    
    

    return;
 }

  
#pragma code highVec = 0x08
void interrupt_at_highVec(void)
{
 _asm GOTO hpISR _endasm
       
}
    
#pragma code
    
#pragma interrupt hpISR
void hpISR(void) {
    
    if (PIR1bits.RCIF)
    {
        
        rx232Isr();
        return;
    }
    else if(PIR1bits.TXIF)
    {
        
        tx232Isr();
        return;
    }
    
    
 } 


 void tx232Isr(void) {
 
    PIE1bits.TXIE = 0;
    
    while(TXREG==1){};//wait till not busy
    TXREG = OUTGOINGdata.DataID;//send first dataID character
    
    //special case if the data is 'e' (unlucky)
    if(OUTGOINGdata.DataValue == 'e')
    {
        while(TXREG==1){};//wait till now busy
        putcUSART('\b');//first send \r so we know the e is genuine data
       
     }
    else
    {
        while(TXREG==1){};//wait till now busy
        TXREG = OUTGOINGdata.DataValue;//first send \r so we know the e is genuine data  
        
    } 
    
   
    return;
 }
 
 
 void rx232Isr(void) {
    
     PIE1bits.RCIE = 0;//turn off receive while we are processing current data
     
     INCOMINGdata.DataID = RCREG;
     
     if(INCOMINGdata.DataID == 'e')
     {
         //get RSSI
         getRSSI();
         return;
     }
     
     while(!DataRdyUSART()){}    //wait till data ready
     
     INCOMINGdata.DataValue = RCREG;   //get data value
     
     if (INCOMINGdata.DataValue == '\b')     //check if the data value is 'e'
     {
         INCOMINGdata.DataValue = 'e';       
     }   
     else if(INCOMINGdata.DataID == 'e')
     {
         
         getRSSI();
         
         return;
     }
  
     PIE1bits.RCIE = 1;      //re-enable receive interrupt
     return;   
 } 
    
 