#include "string.h"

#define UPPER_LINE 0b10000000
#define LOWER_LINE 0b11000000
#define REPLACE 1
#define SELECT_FLAG	INTCON3bits.INT0IF
#define NEXT_FLAG	INTCON3bits.INT1IF
#define FACTORY_FLAG	INTCON3bits.INT2IF
#define NOT_IN_MENU	0
#define SET_MOTOR_ON_OFF 1
#define SET_MAX_SPEED	2
#define SET_AUTO_MODE	3
#define USER_MENU_END 4
#define SET_P_GAIN	1
#define SET_I_GAIN	2
#define SET_D_GAIN	3
#define SET_SPEED_MAX	4
#define SET_YAW_RATE_MAX	5
#define SET_IR_SAMPLES_PER_ESTIMATE	6
#define SET_IR_SAMPLE_RATE	7
#define SET_RF_SAMPLES_PER_ESTIMATE	8
#define FACTORY_MENU_END	9
#define MANUAL 1
#define AUTO 0
#define PRIMARY 0
#define SECONDARY 1
#define USER 1
#define FACTORY 0
#define FOUND_LED PORTCbits.RC0
#define MOVING_LED	PORTCbits.RC3

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
		if(manualOrAuto == MANUAL && userOrFactory == USER){ // and in manual mode and not in factory mode
			switch(userMenuPosition){
				case NOT_IN_MENU : // if not in menu when select button pressed
					userMenuPosition = SET_MOTOR_ON_OFF; // enter menu	
					break;
				case SET_MOTOR_ON_OFF : // if in set motor on/off mode when select button pressed
					toggleMotors = 1; // set flag to switch on/off motors
					break;
				case SET_MAX_SPEED : // if in set max speed mode when select button pressed
					maxSpeed = rotaryEncoder; // set max speed
					break;
				case SET_AUTO_MODE : // if in set auto mode when select button pressed
					manualOrAuto = AUTO; // change to auto mode
					break;
				case USER_MENU_END : // if in switch to secondary UI when select button pressed
					interface1or2 = SECONDARY; // change to secondary UI
				default :
					break;
			}
			
		}
		else if(manualOrAuto == AUTO && userOrFactory == USER){ // and in auto mode and not in factory mode
			findParrotFlag = 1;
		}
		else if(userOrFactory == FACTORY){
			switch(factoryMenuPosition){
				case NOT_IN_MENU : // and in factory mode but not in menu
					factoryMenuPosition = SET_P_GAIN; // enter menu, starting at setting Kp
					break;
				case SET_P_GAIN :
					Kp = rotaryEncoder; // set Kp to value established by rotary encoder
					break;
				case SET_I_GAIN :
					Ki = rotaryEncoder; // set Ki to value established by rotary encoder
					break;
				case SET_D_GAIN :
					Kd = rotaryEncoder; // set Kd to value established by rotary encoder
					break;
				case SET_YAW_RATE_MAX :
					maxYawRate = rotaryEncoder; // set max yaw rate to value established by rotary encoder
					break;
				case SET_IR_SAMPLES_PER_ESTIMATE :
					irSPE = rotaryEncoder; // set IR samples per estimate to value established by rotary encoder
					break;
				case SET_IR_SAMPLE_RATE :
					irFs = rotaryEncoder; // set IR sample rate to value established by rotary encoder
					break;
				case SET_RF_SAMPLES_PER_ESTIMATE :
					rfSPE = rotaryEncoder; // set RF samples per estimate to value established by rotary encoder
					break;
				case FACTORY_MENU_END :
					userOrFactory = USER; // go back to manual mode
					break;
				default :
					break;
			}
		}
		SELECT_FLAG = 0; // clear flag
	}
	else if(NEXT_FLAG){ // if next button pressed
		if(manualOrAuto == MANUAL && userOrFactory == USER){ // in manual mode
			if(userMenuPosition < USER_MENU_END){ // if end of menu not reached
				userMenuPosition++; // go to next menu item
			}
			else{
				userMenuPosition = NOT_IN_MENU; // if end of menu reached, exit menu
			}
		}
		else if(manualOrAuto == AUTO && userOrFactory == USER){ // in auto mode
			manualOrAuto = MANUAL; // next button transitions to manual mode
		}
		else if(userOrFactory == FACTORY){ // in factory mode
			if(factoryMenuPosition < FACTORY_MENU_END){ // if end of menu not reached
				factoryMenuPosition++; // go to next menu item
			}
			else{
				userMenuPosition = SET_P_GAIN; // if end of menu reached, go to start of menu (set P gain)
			}
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

// TODO: configure I/O pins
void configIOCommander(void){
	// PORTA
	TRISAbits.TRISA0 = 1; // fwd-back joystick as input
	TRISAbits.TRISA1 = 1; // L-R joystick as input
	TRISAbits.TRISA2 = 1; // rotary encoder as input
	
	// PORTB
	TRISBbits.TRISB0 = 1; // select button as input
	TRISBbits.TRISB1 = 1; // next button as input
	TRISBbits.TRISB2 = 1; // factory mode button as input
	
	// PORTC
	TRISCbits.TRISC0 = 0; // found LED as output
	TRISCbits.TRISC3 = 0; // moving LED as output
	TRISCbits.TRISC6 = 0; // EUSART Tx as output
	TRISCbits.TRISC7 = 1; // EUSART Rx as input
	
	// PORTD
	TRISCD = 0; // LCD as output
	
	
	
}

// TODO: PUT THIS IN ITS OWN FILE
void dispSigStrength(unsigned char strength){
	char* msg;
	// TODO: convert signal strength to string
	msg[] = strcat("S=",sigStrengthStr); // display signal strength on upper line of LCD
	disp_line(&msg,UPPER_LINE,REPLACE);
}

void commanderLEDs(void){
	if(found){ // if the parrot is found
		FOUND_LED = 1; // turn on found LED
	}
	else{ // while parrot isn't found
		FOUND_LED = 0; // keep found LED off
	}
	
	if(moving){ // if the mobile robot is moving
		MOVING_LED = 1; // turn on moving LED
	}
	else{ // while mobile robot is stationary
		MOVING_LED = 0; // keep moving LED off
	}
}

void askToSetToNewVal(char* str,char* valStr,char* rEnconderStr){
	msg[] = strcat(str,valStr); // display current value on upper line
	disp_line(&msg,UPPER_LINE,REPLACE);
	msg[] = strcat("Set to: ",rEncoderStr); // display value set by rotary encoder on lower line
	disp_line(&msg,LOWER_LINE,REPLACE);
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
				msg[] = strcat("Set to: ",rEncoderStr); // display max speed value being set by rotary encoder on lower line
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_AUTO_MODE : // if in set auto mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg[] = "Enter auto mode?"; // ask the user if they want to enter auto mode
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case USER_MENU_END : // if in switch to secondary UI mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg[] = "Enter UI2?"; // ask the user if they want to enter secondary user interface
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
	
	// LCD displays
    switch(factoryMenuPosition){
		case NOT_IN_MENU : // if FACTORY MODE but not in menu
			msg[] = "FACTORY MODE"; // display FACTORY MODE on upper line of LCD
			disp_line(&msg,UPPER_LINE,REPLACE);
			msg[] = "Enter menu?"; // ask the user if they want to enter the menu
			disp_line(&msg,LOWER_LINE,REPLACE);
			break;
		// TODO: GET GAINS AND CONVERT TO STRINGS
		case SET_P_GAIN :
			msg[] = strcat("Kp = ",KpStr); // display current Kp on upper line
			askToSetToNewVal("Kp = ",KpStr,rEncoderStr); // display current Kp on upper line and display Kp set by rotary encoder on lower line
			break;
		case SET_I_GAIN :
			askToSetToNewVal("Ki = ",KiStr,rEncoderStr); // display current Ki on upper line and display Ki set by rotary encoder on lower line
			break;
		case SET_D_GAIN :
			askToSetToNewVal("Kd = ",KdStr,rEncoderStr); // display current Kd on upper line and display Kd set by rotary encoder on lower line
			break;
		case SET_YAW_RATE_MAX :
			askToSetToNewVal("Max dθ/dt = ",maxYawRateStr,rEncoderStr); // display current max yaw rate on upper line and display max yaw rate set by rotary encoder on lower line
			break;
		case SET_IR_SAMPLES_PER_ESTIMATE :
			askToSetToNewVal("IR SPE = ",irSpeStr,rEncoderStr); // display current IR samples per estimate on upper line and display IR samples per estimate set by rotary encoder on lower line
			break;
		case SET_IR_SAMPLE_RATE :
			askToSetToNewVal("IR fs = ",irFsStr,rEncoderStr); // display current IR sample rate on upper line and display IR sample rate set by rotary encoder on lower line
			break;
		case SET_RF_SAMPLES_PER_ESTIMATE :
			askToSetToNewVal("RF SPE = ",KpStr,rEncoderStr); // display current RF samples per estimate on upper line and display Kp set by rotary encoder on lower line
			break;
		case FACTORY_MENU_END :
			msg[] = "SEL --> Manual"; // display FACTORY MODE on upper line of LCD
			disp_line(&msg,UPPER_LINE,REPLACE);
			msg[] = "NEXT --> Kp"; // ask the user if they want to enter the menu
			disp_line(&msg,LOWER_LINE,REPLACE);
			break;
		default :
			break;
	}
}
