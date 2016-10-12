/* 
 * File:   XbeeSerial.h
 * Author: Angus 
 *
 * Created on 11 October 2016, 3:15 PM
 */

#ifndef XBEESERIAL_H
#define	XBEESERIAL_H

 //declare functions
 void setSerial(void);
 void hpISR(void);
 void tx232Isr(void);
 void rx232Isr(void);
 
 //allocate memory for the structs
 volatile DataStruct INCOMINGdata;
 volatile DataStruct OUTGOINGdata;

#endif	/* XBEESERIAL_H */

