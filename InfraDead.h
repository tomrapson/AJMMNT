

#ifndef INFRADEAD_H
#define	INFRADEAD_H

#include <p18cxxx.h>
#include "SerialStuff.h"

//VARS FOR MAIN

/*Functions initialize*/
int IRget(unsigned char IRchoose); //read ADC 4 times, storing in buffer
//void IRinitialise(void);

#endif	/* INFRADEAD_H */

