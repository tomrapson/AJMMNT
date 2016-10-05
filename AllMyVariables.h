
 //* @file AllMyVariables.h
 //* @author Team Elizabeth
// * @date 5th october 2016
 //* @brief This defines all of our variables



#ifndef ROBOT_VARS_H
#define	ROBOT_VARS_H

#define BUF_SZ          26 
#define MaxIRElements   255
#define LOST_CONNECTION 570 //Equivalent to 15seconds


char receivedValues[BUF_SZ];

typedef struct
{
    char DataID;
    unsigned char DataValue;
    
}DataStruct;

//Declaring my two different Structures
volatile DataStruct INCOMINGdata;        
volatile DataStruct OUTGOINGdata;


//The GLOBAL variables needed by different modules
volatile unsigned char AutoSTATE;     //boolean values
volatile unsigned char ParrotSTATE;
volatile unsigned char MotorSTATE;
volatile unsigned char SpeedCURR;     //0-255 values
volatile unsigned char SpeedMAX;     
volatile unsigned char YawCURR;
volatile unsigned char YawMAX; 
volatile unsigned char GainD;
volatile unsigned char GainI;
volatile unsigned char GainP;
volatile unsigned char RFsample;
volatile unsigned char IRsample;
volatile unsigned char IRrate;

volatile unsigned char SENDdataID;


/*#pragma udata large_udata
unsigned char IRArray[MaxIRElements];
#pragma udata*/





unsigned char direction = FORWARD;
unsigned char rawinput1 = 200;
unsigned char rawinput2 = 200;
unsigned char connection = 0x00;
unsigned char datasend = 30;
unsigned char realinput1;
unsigned char realinput2;
unsigned char wheelsel1;
unsigned char wheelsel2;


#endif	/* XC_HEADER_TEMPLATE_H */

