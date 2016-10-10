#include "string.h"

#define UPPER_LINE 0b10000000
#define LOWER_LINE 0b11000000
#define REPLACE 1
#define SELECT_FLAG	INTCON3bits.INT0IF
#define NEXT_FLAG	INTCON3bits.INT1IF
#define FACTORY_FLAG	INTCON3bits.INT2IF
#define MAX_MENU_POS 4
#define NOT_IN_MENU	0
#define SET_MOTOR_ON_OFF 1
#define SET_MAX_SPEED	2
#define SET_AUTO_MODE	3
#define SET_2_INTERFACE 4
#define SET_PID_GAINS	1
#define SET_SPEED_MAX	2
#define SET_YAW_RATE_MAX	3
#define SET_IR_SAMPLES_PER_ESTIMATE	4
#define SET_IR_SAMPLE_RATE	5
#define SET_RF_SAMPLES_PER_ESTIMATE	6
#define SWITCH_TO_MANUAL	7
#define MANUAL 1
#define AUTO 0
#define PRIMARY 0
#define SECONDARY 1
#define USER 1
#define FACTORY 0
#define GREEN_FOUND PORTCbits.RC0
#define RED_FOUND	PORTCbits.RC3
#define GREEN_MOVING	PORTCbits.RC4
#define RED_MOVING	PORTCbits.RC5

void findParrot(void);



unsigned char userOrFactory = USER; // flag set to 1 in user mode, 0 in factory mode
unsigned char manualOrAuto = MANUAL; // flag set to 1 in manual mode, 0 in auto mode
unsigned char found = 0; // flag set to 1 when parrot found, 0 if not
unsigned char moving = 0; // flag set to 1 when mobile robot is moving, 0 if stationary
unsigned char interface1or2 = PRIMARY; // flag set to 1 in secondary UI, 0 in primary UI
unsigned char motors = 0; // flag set to 1 if motors on, 0 if off
unsigned char toggleMotors = 0; // flag set to 1 if motors need to be toggled, 0 if not
unsigned char userMenuPosition = NOT_IN_MENU; // counter to determine position in the menu
unsigned char factoryMenuPosition = NOT_IN_MENU
unsigned char findParrotFlag = 0; // flag set to 1 to trigger search for parrot, 0 if not

unsigned char maxSpeed;
unsigned char* maxSpeedStr;
unsigned char sigStrength;
unsigned char* sigStrengthStr;
unsigned char* msg;

// TODO: configure interrupts
// TODO: configure I/O pins
// TODO: write to found and moving flags

#pragma code highPriorityInterruptAddress=0x0008
void high_interrupt(void){
    highPriorityIsr();
}

#pragma code lowPriorityInterruptAddress=0x0018
void low_interrupt(void){
    lowPriorityIsr();
}

#pragma code

#pragma interrupt highPriorityIsr
void highPriorityIsr(void){ 
	// TODO: DISABLE INTERRUPTS OF SAME PRIORITY

	// External button interrupt
	if(SELECT_FLAG){ // if select button pressed
		SELECT_FLAG = 0; // reset flag
		if(manualOrAuto == MANUAL){ // and in manual mode
			switch(userMenuPosition){
				case NOT_IN_MENU : // if not in menu when select button pressed
					userMenuPosition = SET_MOTOR_ON_OFF; // enter menu	
					break;
				case SET_MOTOR_ON_OFF : // if in set motor on/off mode when select button pressed
					toggleMotors = 1; // set flag to switch on/off motors
					break;
				case SET_MAX_SPEED : // if in set max speed mode when select button pressed
					maxSpeed = rotary_encoder; // set max speed
					break;
				case SET_AUTO_MODE : // if in set auto mode when select button pressed
					manualOrAuto = AUTO; // change to auto mode
					break;
				case SET_2_INTERFACE : // if in switch to secondary UI when select button pressed
					interface1or2 = SECONDARY; // change to secondary UI
				default :
					break;
			}
			
		}
		else{ // and in auto mode
			findParrotFlag = 1;
		}
		SELECT_FLAG = 0; // clear flag
	}
	else if(NEXT_FLAG){ // if next button pressed
		if(manualOrAuto){ // in manual mode
			if(userMenuPosition < MAX_MENU_POS){
				userMenuPosition++; // if end of menu not reached, go to next menu item
			}
			else{
				userMenuPosition = NOT_IN_MENU; // if end of menu reached, exit menu
			}
		}
		else{ // in auto mode
			manualOrAuto = MANUAL; // next button transitions to manual mode
		}
		NEXT_FLAG = 0; // clear flag
	}
	else if(FACTORY_FLAG){ // if factory switch pressed
		userOrFactory = FACTORY; // switch to factory mode
		FACTORY_FLAG = 0; // clear flag
	}
}

#pragma interrupt lowPriorityIsr
void lowPriorityIsr(void){
	// TODO: DISABLE INTERRUPTS OF SAME PRIORITY
}

// TODO: write ISR to convert rotary encoder A/D

// TODO: PUT THIS IN ITS OWN FILE
void dispSigStrength(unsigned char strength){
	char* msg;
	// TODO: convert signal strength to string
	msg[] = strcat("S=",sigStrengthStr); // display signal strength on upper line of LCD
	disp_line(&msg,UPPER_LINE,REPLACE);
}

void commanderLEDs(void){
	if(found){ // if the parrot is found
			GREEN_FOUND = 1; // turn on green found LED
			RED_FOUND = 0; // turn off red found LED
		}
		else{ // while parrot isn't found
			GREEN_FOUND = 0; // keep green found LED off
			RED_FOUND = 1; // keep red found LED off
		}
		
		if(moving){ // if the mobile robot is moving
			GREEN_MOVING = 1; // turn on green moving LED
			RED_MOVING = 0; // turn off red moving LED
		}
		else{ // while mobile robot is stationary
			GREEN_MOVING = 0; // keep green moving LED off
			RED_MOVING = 1; // keep red moving LED off
		}
}

while(manualOrAuto == MANUAL &&){
	// TODO: GET SIGNAL STRENGTH
		
		if(toggleMotors){ // if motors need to be toggled
			if(motors){ // and they are on
				// TODO: motion control set points set to 0
                // TODO: motor drive amplifiers on Mobile Robot de-energised
			}
			else{ // and they are off
				// TODO: turn them on
			}
			motors = !motors; // toggle motors flag
			changeMotors = 0; // clear flag
		}
		
    if(interface1or2 == PRIMARY){
        // LCD displays in various menu positions
		switch(userMenuPosition){
			case NOT_IN_MENU : // if not in menu
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg[] = "USER MANUAL MODE"; // display USER MANUAL MODE on lower line of LCD
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_MOTOR_ON_OFF : // if in set motor on/off mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				if(motors){ // if motors are on
					msg[] = "Turn motors off?"; // ask user if they want to turn them off
				}
				else{ // if motors are off
					msg[] = "Turn motors on?"; // ask user if they want to turn them on
				}
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_MAX_SPEED : // if in set max speed mode
				// TODO: GET MAX SPEED VALUE FROM ROTARY ENCODER AND CONVERT TO STRING
				// display max speed and signal strength on upper line
				msg[] = strcat("MaxSpd=",maxSpeedStr);
				msg[] = strcat(msg," S=");
				msg[] = strcat(msg,sigStrengthStr);
				disp_line(&msg,UPPER_LINE,REPLACE);
				
				msg[] = strcat("Set to: ",maxSpeedStr); // display max speed value being set by rotary encoder on lower line
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_AUTO_MODE : // if in set auto mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg[] = "Enter auto mode?"; // ask the user if they want to enter auto mode
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_2_INTERFACE : // if in switch to secondary UI mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg[] = "Enter 2ยบ UI?"; // ask the user if they want to enter secondary user interface
				disp_line(&msg,LOWER_LINE,REPLACE);
			default :
				break;
		}
		
		// LEDs (implemented without logic gates here...)
		commanderLEDs();
                 
        
    }
	
	// TODO: write manual mode secondary UI
	else{
		
	}
}

while(manualOrAuto == AUTO){
	if(interface1or2 == PRIMARY){
		// LCD display
		dispSigStrength(sigStrength); // display signal strength on upper line
		msg[] = "AUTO MODE"; // display AUTO MODE on lower line of LCD
		disp_line(msg,LOWER_LINE,REPLACE);
		
		// LEDs (implemented without logic gates here...)
		commanderLEDs();
	}
	
	// TODO: write auto mode secondary UI
	else{
		
	}
	if(findParrotFlag){ // if findParrotFlag is set, commence search
		findParrot();
	}
}

// TODO: write factory mode
while(userOrFactory == FACTORY){
    switch(factoryMenuPosition){
	}
}
