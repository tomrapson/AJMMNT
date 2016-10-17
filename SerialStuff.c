/**
 * @file SerialFunctions.c
 * @author Team Elizabeth
 * @date 5th october 2016
 * @brief This is the c file for defining our functions that deal with incoming/
 * outgoing Serial data.
 * 
 */

#include <p18f4520.h>
#include "SerialStuff.h"


//The GLOBAL variables needed by different modules
volatile unsigned char AutoSTATE=0;     //boolean values
volatile unsigned char ParrotSTATE=0;
volatile unsigned char MotorSTATE=0;
volatile unsigned char SpeedMAX=0;     //0-255 values
volatile unsigned char RIGHTspeed=0;
volatile unsigned char LEFTspeed=0;     
volatile unsigned char YawMAX=0; 
volatile unsigned char GainD=0;
volatile unsigned char GainI=0;
volatile unsigned char GainP=0;
volatile unsigned char RFsample=0;
volatile unsigned char IRsample=0;
volatile unsigned char IRrate=0;
volatile unsigned char SENDdataID=0; 
volatile unsigned char  RSSI;             //RSSI value

volatile struct DataStruct INCOMINGdata;        
volatile struct DataStruct OUTGOINGdata;




//Takes the current value that has come through the serial and assigns it to the
//correct variable
//Unused ASCII chars have been commented out
void RECEIVEserialPARSE( void )
{
    switch(INCOMINGdata.DataID)
        {
            case 'a':           //sets auto mode 1 ON 0 OFF. HAS EXTRA BOOLEAN CHECK
                if (INCOMINGdata.DataValue == 1 || INCOMINGdata.DataValue ==0){
                    AutoSTATE = INCOMINGdata.DataValue;
                }
                break;
            case 'b':           //SPEED OF LEFT WHEEL
                RIGHTspeed = INCOMINGdata.DataValue;
                break;
            case 'c':           //SPEED OF RIGHT WHEEL
                LEFTspeed = INCOMINGdata.DataValue;
                break;
            case 'd':           //PID Gain Diff
                GainD = INCOMINGdata.DataValue;
                break;
            /*case 'e':           
                //EMPTY
                break;*/
            case 'f':           //RF Samples per estimate
                RFsample = INCOMINGdata.DataValue;
                break;
            case 'g':           //IR Samples per estimate
                IRsample = INCOMINGdata.DataValue;
                break;
            /*case 'h':           
               //EMPTY
                break;*/
            case 'i':           //PID Gain Intergral
                GainI = INCOMINGdata.DataValue;
                break;
            /*case 'j':          
                //EMPTY
                break;
            case 'k':          
               //EMPTY
                break;
            case 'l':          
               //EMPTY
                break;*/
            case 'm':           //PARROT MOVEMENT stationary '0' moving '1'
                if (INCOMINGdata.DataValue == 1 || INCOMINGdata.DataValue ==0){
                    ParrotSTATE = INCOMINGdata.DataValue;
                }
                break;
            /*case 'n':           
               //EMPTY
                break;*/
            case 'o':           //MOTORS ON/OFF
                if (INCOMINGdata.DataValue == 1 || INCOMINGdata.DataValue ==0){
                    MotorSTATE = INCOMINGdata.DataValue;
                }
                break;
            case 'p':           //PID Gain Proportional
                GainP = INCOMINGdata.DataValue;
                break;
            /*case 'q':           
               //EMPTY
                break;*/
            case 'r':           //IR Sample Rate
                IRrate = INCOMINGdata.DataValue;
                break;
            case 's':           //MAX SPEED
                SpeedMAX = INCOMINGdata.DataValue;
                break;
            /*case 't':           
               //EMPTY
                break;
            case 'u':           
               //EMPTY
                break;
            case 'v':           
               //EMPTY
                break;
            case 'w':           
               //EMPTY
                break;
            case 'x':           
               //EMPTY
                break;*/
            case 'y':           //MAX YAW
                YawMAX = INCOMINGdata.DataValue;
                break;
            /*case 'z':           //v for velocity DataValue
                //EMPTY
                break;               */      
            default:            //if DataID not recognised
                break;
        }
}

//Takes an ASCII DataID char, and stores the OUTGOING data in a struct
void SENDserialPACKAGE( void  )
{
    switch(SENDdataID)
        {
            case 'a':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'b':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) RIGHTspeed;
                 break;
            case 'c':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) LEFTspeed;
                 break;
            case 'd':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) GainD;
                 break;
            /*case 'e':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            case 'f':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) RFsample;
                 break;
            case 'g':           
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) IRsample;
                 break;
            /*case 'h':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            case 'i':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) GainI;
                 break;
            /*case 'j':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'k':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'l':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            case 'm':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) ParrotSTATE;
                 break;
            /*case 'n':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            case 'o':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) MotorSTATE;
                 break;
            case 'p':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) GainP;
                 break;
            /*case 'q':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;                 */
            case 'r':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) IRrate;
                 break;
            case 's':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) SpeedMAX;
                 break;
            /*case 't':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'u':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'v':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'w':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;
            case 'x':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            case 'y':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) YawMAX; 
                 break;
            /*case 'z':           //sets auto mode 1 ON 0 OFF
            OUTGOINGdata.DataID = SENDdataID;
            OUTGOINGdata.DataValue = (unsigned char) AutoSTATE;
                 break;*/
            default:            //if DataID not recognised
                break;
      
        }
}

