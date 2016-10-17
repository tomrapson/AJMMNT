/**
 * @file SerialFunctions.c
 * @author Team Elizabeth
 * @date 5th october 2016
 * @brief This is the h file for defining our functions that deal with incoming/
 * outgoing Serial data.
 * 
 */

#ifndef SERIALSTUFF_H
#define	SERIALSTUFF_H

struct DataStruct
{
    char DataID;
    unsigned char DataValue;
    
} ;

extern volatile struct DataStruct INCOMINGdata;        
extern volatile struct DataStruct OUTGOINGdata;

//DECARINGGGGG
//The GLOBAL variables needed by different modules
extern volatile unsigned char AutoSTATE;     //boolean values
extern volatile unsigned char ParrotSTATE;
extern volatile unsigned char MotorSTATE;
extern volatile unsigned char SpeedMAX;     
extern volatile unsigned char RIGHTspeed;
extern volatile unsigned char LEFTspeed;     //0-255 values
extern volatile unsigned char YawMAX; 
extern volatile unsigned char GainD;
extern volatile unsigned char GainI;
extern volatile unsigned char GainP;
extern volatile unsigned char RFsample;
extern volatile unsigned char IRsample;
extern volatile unsigned char IRrate;
extern volatile unsigned char SENDdataID;        //This is the variable we set when sending data
extern volatile unsigned char  RSSI;             //RSSI value

//My send/receive functions
void RECEIVEserialPARSE(void);
void SENDserialPACKAGE(void);
void GETserial(void);

#endif	/* SERIALSTUFF_H */

