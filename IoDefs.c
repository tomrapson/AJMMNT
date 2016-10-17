/*Author: Tristan Hall from House Gilpin*/
/**/

/*Required include files*/
#include <p18f4520.h>
#include "IoDefs.h"

void ConfigIO(void)
{
    /*PORTA*/
    TRISAbits.RA4 = 1; //Input to COUNTER module (LEFT WHEEL)
    
    /*PORTB*/
    TRISBbits.TRISB0 = 0;    //OUTPUT TO DEBUG
    TRISBbits.TRISB1 = 1;   //INPUT EXTERNAL INTERRUPT
    TRISBbits.TRISB2 = 1;   //INPUT EXTERNAL INTERRUPT
    
    /*PORTC bit 1 and 2 is an output*/
    TRISCbits.TRISC2 = 0;   //CCP1
    TRISCbits.TRISC1 = 0;   //CCP2
    TRISCbits.TRISC0 = 1;   //Input to COUNTER module (RIGHT WHEEL)

    
    TRISCbits.TRISC6 = 0;   /*Set required TRISC for serial*/
    TRISCbits.TRISC7 = 1;
    
    /*PORTD*/
    /*Need 6 output pins at portD for each of the enable bits*/
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    
    /*PORTE*/
}