#include "string.h"

#define UPPER_LINE 0b10000000
#define LOWER_LINE 0b11000000
#define REPLACE 1
#define FACTORY_FLAG	INTCON3bits.INT0IF
#define SELECT_FLAG	INTCON3bits.INT1IF
#define RIGHT_FLAG	INTCON3bits.INT2IF
#define LEFT_FLAG	INTCONbits.RBIF
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
#define MAX_SIGNAL_STRENGTH_DIGITS 4
#define MAX_MAX_SPEED_DIGITS	3
#define MAX_LCD_CHARS

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
unsigned char maxSpeedStr[MAX_MAX_SPEED_DIGITS];
unsigned char sigStrength;
unsigned char sigStrengthStr[MAX_SIGNAL_STRENGTH_DIGITS];
unsigned char msg[MAX_LCD_CHARS];

// TODO: configure interrupts
// TODO: write to found and moving flags
// TODO: LEFT BUTTON

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
					maxSpeed = readRotaryEncoder(); // set max speed
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
					Kp = readRotaryEncoder(); // set Kp to value established by rotary encoder
					break;
				case SET_I_GAIN :
					Ki = readRotaryEncoder(); // set Ki to value established by rotary encoder
					break;
				case SET_D_GAIN :
					Kd = readRotaryEncoder(); // set Kd to value established by rotary encoder
					break;
				case SET_YAW_RATE_MAX :
					maxYawRate = readRotaryEncoder(); // set max yaw rate to value established by rotary encoder
					break;
				case SET_IR_SAMPLES_PER_ESTIMATE :
					irSPE = readRotaryEncoder(); // set IR samples per estimate to value established by rotary encoder
					break;
				case SET_IR_SAMPLE_RATE :
					irFs = readRotaryEncoder(); // set IR sample rate to value established by rotary encoder
					break;
				case SET_RF_SAMPLES_PER_ESTIMATE :
					rfSpe = readRotaryEncoder(); // set RF samples per estimate to value established by rotary encoder
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
	else if(RIGHT_FLAG){ // if right button pressed
		if(manualOrAuto == MANUAL && userOrFactory == USER){ // in manual mode
			if(userMenuPosition < USER_MENU_END){ // if end of menu not reached
				userMenuPosition = ; // go to next menu item
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

// TODO: configure interrupts
void configInterrupts(void){
	
}

void configAD(void){
	ADCON0 = 0; // AD off
	ADCON1 = 0b00001011; // AN3:0 Analog input; rest digital
	ADCON2 = 0; // Left justified to allow simple 8-bit read, 0 T_AD, Fosc/2
}

void num2str(unsigned char *buf, unsigned char number){
    unsigned char i, remainder, length = 0, n;
 
    n = number;
	
	// find number of digits
    while (n != 0)
    {
        length++;
        n /= 10;
    }
	
	// store each digit as a character in the buffer
    for (i = 0; i < length; i++)
    {
        remainder = number % 10;
        number = number / 10;
        buf[length - (i + 1)] = remainder + '0';
    }
	
    buf[length] = '\0'; // append a null character
}

unsigned char readFBJoystick(void){
	ADCON0 = 0b00000011; // Channel 0 (AN0,RA0), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
}

unsigned char readLRJoystick(void){
	ADCON0 = 0b00000111; // Channel 1 (AN0,RA1), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
}

unsigned char readRotaryEncoder(void){
	ADCON0 = 0b00001011; // Channel 2 (AN2/RA0), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
}

void getRotaryEncoderStr(void){
	rotaryEncoder = readRotaryEncoder(); // get new value from rotary encoder
	num2str(rotaryEncoderStr,rotaryEncoder); // convert value to a string
}

void dispSigStrength(unsigned char strength){
	unsigned char msg[MAX_LCD_CHARS];
	num2str(sigStrengthStr,sigStrength); // convert signal strength to a string
	msg = strcat("S=",sigStrengthStr); // display signal strength on upper line of LCD
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

void askToSetToNewVal(unsigned char* variable,unsigned char* curVal,unsigned char* rotaryEncoder){
	unsigned char msg[MAX_LCD_CHARS];
	msg = strcat(variable,curVal); // display current value on upper line
	disp_line(&msg,UPPER_LINE,REPLACE);
	msg = strcat("Set to: ",rotaryEncoder); // display value set by rotary encoder on lower line
	disp_line(&msg,LOWER_LINE,REPLACE);
}

while(manualOrAuto == MANUAL){
	// TODO: GET SIGNAL STRENGTH
	
	FBJoystick = readFBJoystick(); // update fwd-back joystick value
	LRJoystick = readLRJoystick(); // update left-right joystick value
		
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
				msg = "USER MANUAL MODE"; // display USER MANUAL MODE on lower line of LCD
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_MOTOR_ON_OFF : // if in set motor on/off mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				if(motors){ // if motors are on
					msg = "Turn motors off?"; // ask user if they want to turn them off
				}
				else{ // if motors are off
					msg = "Turn motors on?"; // ask user if they want to turn them on
				}
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_MAX_SPEED : // if in set max speed mode
				num2str(maxSpeedStr,maxSpeed); // convert max speed to a string
				// display max speed and signal strength on upper line
				msg = strcat("MaxSpd=",maxSpeedStr);
				msg = strcat(msg," S=");
				msg = strcat(msg,sigStrengthStr);
				disp_line(&msg,UPPER_LINE,REPLACE);
				getRotaryEncoderStr(); // get new max speed as a string from rotary encoder
				msg = strcat("Set to: ",rotaryEncoderStr); // display max speed value being set by rotary encoder on lower line
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case SET_AUTO_MODE : // if in set auto mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg = "Enter auto mode?"; // ask the user if they want to enter auto mode
				disp_line(&msg,LOWER_LINE,REPLACE);
				break;
			case USER_MENU_END : // if in switch to secondary UI mode
				dispSigStrength(sigStrength); // display signal strength on upper line
				msg = "Enter UI2?"; // ask the user if they want to enter secondary user interface
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
		msg = "AUTO MODE"; // display AUTO MODE on lower line of LCD
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

while(userOrFactory == FACTORY){
	getRotaryEncoderStr(); // get new rotary encoder value as a string
	// LCD displays
    switch(factoryMenuPosition){
		case NOT_IN_MENU : // if FACTORY MODE but not in menu
			msg = "FACTORY MODE"; // display FACTORY MODE on upper line of LCD
			disp_line(&msg,UPPER_LINE,REPLACE);
			msg = "Enter menu?"; // ask the user if they want to enter the menu
			disp_line(&msg,LOWER_LINE,REPLACE);
			break;
		// TODO: GET GAINS
		case SET_P_GAIN :
			num2str(KpStr,Kp); // convert Kp to a string
			
			num2str(KpStr,Kp); // convert Kp to a string
			msg = strcat("Kp = ",KpStr); // display current Kp on upper line
			askToSetToNewVal("Kp = ",KpStr,rotaryEncoderStr); // display current Kp on upper line and display Kp set by rotary encoder on lower line
			break;
		case SET_I_GAIN :
			num2str(KiStr,Ki); // convert Ki to a string
			askToSetToNewVal("Ki = ",KiStr,rotaryEncoderStr); // display current Ki on upper line and display Ki set by rotary encoder on lower line
			break;
		case SET_D_GAIN :
			num2str(KdStr,Kd); // convert Kd to a string
			askToSetToNewVal("Kd = ",KdStr,rotaryEncoderStr); // display current Kd on upper line and display Kd set by rotary encoder on lower line
			break;
		case SET_YAW_RATE_MAX :
			num2str(maxYawRateStr,maxYawRate); // convert max yaw rate to a string
			askToSetToNewVal("Max dθ/dt = ",maxYawRateStr,rotaryEncoderStr); // display current max yaw rate on upper line and display max yaw rate set by rotary encoder on lower line
			break;
		case SET_IR_SAMPLES_PER_ESTIMATE :
			num2str(irSpeStr,irSpe); // convert IR samples per estimate to a string
			askToSetToNewVal("IR SPE = ",irSpeStr,rotaryEncoderStr); // display current IR samples per estimate on upper line and display IR samples per estimate set by rotary encoder on lower line
			break;
		case SET_IR_SAMPLE_RATE :
			num2str(irFsStr,irFs); // convert IR sample rate to a string
			askToSetToNewVal("IR fs = ",irFsStr,rotaryEncoderStr); // display current IR sample rate on upper line and display IR sample rate set by rotary encoder on lower line
			break;
		case SET_RF_SAMPLES_PER_ESTIMATE :
			num2str(rfSpeStr,rfSpe); // convert Kp to a string
			askToSetToNewVal("RF SPE = ",rfSpeStr,rotaryEncoderStr); // display current RF samples per estimate on upper line and display Kp set by rotary encoder on lower line
			break;
		case FACTORY_MENU_END :
			msg = "SEL --> Manual"; // display FACTORY MODE on upper line of LCD
			disp_line(&msg,UPPER_LINE,REPLACE);
			msg = "NEXT --> Kp"; // ask the user if they want to enter the menu
			disp_line(&msg,LOWER_LINE,REPLACE);
			break;
		default :
			break;
	}
}
