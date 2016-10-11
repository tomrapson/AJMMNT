#include <stdio.h>
#include <stdlib.h>

#include "pconfig.h"
#include "p18f452.h"

/*Removes warning message when run*/
#pragma config OSC = ECIO, WDT = OFF, LVP = OFF


/*Function prototypes*/
void config(void);
void tx232C(unsigned char * txPtr);
void rx232Isr(void);
void transmitWait(void);
unsigned char stringToNum(unsigned char * userInput);
void numToString(unsigned char * strPtr, unsigned char number);

#pragma idata idata1
// String for welcome message
unsigned char welcome_message[] = "Welcome!\0";
unsigned char *welcome;

// Strings required in Manual Mode
unsigned char menu_string1[] = "Press 'p' to find the parrot. \n\rPress 'a' to go to automatic mode. \0";
unsigned char menu_string2[] = "Press 'f' to go to factory mode. \n\rPress 's' to set max speed. \n\rPress 'g' to turn motors on/off. \n\rPress 'h' for help. \0";
// ANOTHER MESSAGE FOR CHANGING TO PRIMARY USER INTERFACE


#pragma idata idata2
unsigned char maxspeed_string[] = "Enter maximum speed: \0"; 
unsigned char maxspeed_new[] = "Maximum speed is now: \0";
unsigned char *menu1, *menu2, *maxSpeed, *maxSpeedNew;

// Strings for error messages
unsigned char invalid_input[] = "Invalid input! Please enter a number between 0 and 255:\0";
unsigned char *error;
 


/*Global variables*/
//unsigned char firstString[] = "You said: \0";
//unsigned char *printfirstString;
unsigned char *receiveString, *strPtr;
unsigned char buffer[50], stringBuf[50];
//unsigned char *txPtr;
int flag= 0, entryFlag = 0, readCount = 0, firstTime = 0, errorFlag = 0;
unsigned char maxSpeedVal = 0;
unsigned char temp = 0;

/*Interrupt*/
#pragma code HIGH_INTERRUPT_VECTOR = 0x08

void high_ISR (void)
{
    _asm    goto rx232Isr    _endasm
}
#pragma code
/********************************************************/
/*Main*/
/********************************************************/
void main(void)
{
        config();

        tx232C(welcome);
        tx232C(menu1);
        tx232C(menu2);
        
        /*Reset pointers*/
        welcome = welcome_message;
        menu1 = menu_string1;
        menu2 = menu_string2;
        
	while(1)
	{
		/*Flag check*/
		if(flag == 1)
		{
			
            if(entryFlag == 1){
                maxSpeedVal = stringToNum(receiveString);
                if(errorFlag == 0){
                    
                    // transmit 
                    tx232C(maxSpeedNew);
                    maxSpeedNew = maxspeed_new;
                    numToString(strPtr, maxSpeedVal);
                    strPtr = stringBuf;
                    tx232C(strPtr);
                    strPtr = stringBuf;
                }
                
            }
            
            if(buffer[0]=='s'){
                tx232C(maxSpeed);
                maxSpeed = maxspeed_string;
                entryFlag = 1;
            } 
  
		}
	}
	

}
#pragma code
unsigned char stringToNum(unsigned char * userInput){
    
    
	TXSTAbits.TXEN = 0;	 /*Disable transmission*/
    PIE1bits.RCIE = 0;	/*Receive interrupt disabled*/

   
    if(firstTime == 0){
        
        readCount--;
        firstTime = 1;
    }
    
            
    if(readCount == 3){
        
        // check that all input numbers are valid
        if(userInput[0]>57 || userInput[0]<48 || userInput[1]>57 || userInput[1]<48 || userInput[2]>57 || userInput[2]<48){
            tx232C(error);
            error = invalid_input;
            temp = 0;
            errorFlag = 1; 
        } else if(userInput[0]>50 || (userInput[0]==50 && userInput[1]>53) || (userInput[0]==50 && userInput[1]==53 && userInput[2]>53)){ // Error check for 255
            tx232C(error);
            error = invalid_input;
            temp = 0;
            errorFlag = 1;
        } else {
            temp = (userInput[0]-0x30)*0x64 + (userInput[1]-0x30)*0x0A + (userInput[2]-0x30);
            entryFlag = 0;
            errorFlag = 0;  
        }
    } else if (readCount == 2){
        
        if(userInput[0]>57 || userInput[0]<48 || userInput[1]>57 || userInput[1]<48){
            tx232C(error);
            error = invalid_input;
            temp = 0;
            errorFlag = 1;   
        } else {
            temp = (userInput[0]-0x30)*0x0A + (userInput[1]-0x30);
            entryFlag = 0;
            errorFlag = 0;
        }
    } else if (readCount == 1){
        if(userInput[0]>57 || userInput[0]<48){
            tx232C(error);
            error = invalid_input;
            temp = 0;
            errorFlag = 1; 
        } else{
            temp = userInput[0]-0x30;
            entryFlag = 0;
            errorFlag = 0;
        }
    } else {
        
        // transmit "Invalid input! Please enter a number between 0 and 255:"
        tx232C(error);
        error = invalid_input; // reset pointer
        errorFlag = 1;
    }
    
    readCount = 0;
    PIE1bits.RCIE = 1;	/*Receive interrupt enabled*/
     
    return temp;
    
}

void numToString(unsigned char * strPtr, unsigned char number){
    int i, remainder, length = 0, n;
 
    n = number;
    while (n != 0)
    {
        length++;
        n /= 10;
    }
    for (i = 0; i < length; i++)
    {
        remainder = number % 10;
        number = number / 10;
        stringBuf[length - (i + 1)] = remainder + '0';
    }
    stringBuf[length] = '\0';
}

void config(void)
{
    /* Pointer setup */
    //printfirstString= firstString;
    receiveString = buffer;
    strPtr = stringBuf;
    welcome = welcome_message;
    menu1 = menu_string1;
    menu2 = menu_string2;
    maxSpeed = maxspeed_string;
    maxSpeedNew = maxspeed_new;
    error = invalid_input;
    
    // Reset counters
    readCount = 0;
    
	/*Baud rate 9600*/
    //BAUDCONbits.BRG16 = 0;
	//SPBRG = 64;            
	SPBRG = 25;
    
    /*TRANSMIT*/
	TXSTA = 0b00000100;	/*Set BRGH: Asynchronous High speed, clear SYNC Asynchronous mode, 8bit transmission*/
			
	/*RECEIVE*/
	RCSTA = 0b10010000;	/*SPEN enabled(Serial Port Enable)*/
	PIE1bits.RCIE = 1;	/*Receive interrupt enable*/
	
	/*INTERRUPT ENABLE*/
	INTCONbits.GIE = 1;     /*Enable global and interrupts*/
    INTCONbits.PEIE = 1;    /*Enable peripheral interrupts*/
}


void tx232C(unsigned char * txPtr)
{
	
    /*Enable transmission*/
	TXSTAbits.TXEN = 1;
		
	/*Print message*/
	while(*txPtr!= 0x00)
    {
        TXREG = *txPtr;
        transmitWait();
        txPtr++;
    }
    

    /*Carriage reached*/
    if(*txPtr == 0x00)
    {
	/*New line*/
        TXREG = '\n';
        transmitWait();

	/*carriage return*/
        TXREG = '\r';
        transmitWait();
    }
    /*reset flag*/
    flag = 0;
				
	/*Disable transmission*/
	TXSTAbits.TXEN = 0;				
}

/*ISR for receive*/
#pragma interrupt rx232Isr
void rx232Isr(void)
{
	/*Store character into buffer*/
	*receiveString = RCREG;
//    txPtr = receiveString;

	/*If carriage return reached*/
	if(*receiveString == 0x0D)
	{
		/*Set flag*/
		flag = 1;

                /*Append line feed*/
                *(receiveString+1) = '\0';
                receiveString = buffer;
	}
	else
	{
		/*Increment pointer*/
		receiveString++;
        readCount++;
	}
	/*Reset flag*/
	PIR1bits.RCIF = 0;

}

void transmitWait(void)
{
	/*Wait until transmission is complete*/
	while(TXSTAbits.TRMT ==0)
	{}
}