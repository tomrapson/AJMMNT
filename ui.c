#include "string.h"
#include "p18f4520.h"
#include "SerialStuff.h"
#include "lcd.h"

#define UPPER_LINE 0b10000000
#define LOWER_LINE 0b11000000
#define KEEP    0
#define REPLACE 1
#define FACTORY_BUT	PORTBbits.RB4
#define SELECT_BUT	PORTBbits.RB0
#define RIGHT_BUT	PORTBbits.RB2
#define LEFT_BUT	PORTBbits.RB3
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
#define MAX_ROTARY_ENCODER_DIGITS	3
#define MAX_LCD_CHARS
#define SPEED_RIGHT_DATA_ID	'b'
#define SPEED_LEFT_DATA_ID	'c'
#define KP_DATA_ID	'p'
#define KD_DATA_ID	'd'
#define RF_SPE_DATA_ID	'f'
#define IR_SPE_DATA_ID	'g'
#define MOTOR_STATE_DATA_ID	'o'
#define IR_FS_DATA_ID	'r'
#define SPEED_MAX_DATA_ID	's'
#define YAW_RATE_MAX_DATA_ID	'y'
#define MOVING_DATA_ID	'm'
#define FOUND_DATA_ID	'z'
#define SIGNAL_STRENGTH_DATA_ID	'x'
#define START_DATA_ID	'w'
// TODO: signal strength, start finding parrot, parot found defines

/*Define the interrupts*/
void low_interrupt( void );
void high_interrupt( void );
void lowPriorityIsr( void );
void highPriorityIsr( void );

unsigned char userOrFactory = USER; // flag set to 1 in user mode, 0 in factory mode
unsigned char manualOrAuto = MANUAL; // flag set to 1 in manual mode, 0 in auto mode
unsigned char found = 0; // flag set to 1 when parrot found, 0 if not
unsigned char moving = 0; // flag set to 1 when mobile robot is moving, 0 if stationary
unsigned char interface1or2 = PRIMARY; // flag set to 1 in secondary UI, 0 in primary UI
unsigned char motors = 0; // flag set to 1 if motors on, 0 if off
unsigned char toggleMotors = 0; // flag set to 1 if motors need to be toggled, 0 if not
unsigned char userMenuPosition = NOT_IN_MENU; // counter to determine position in the menu
unsigned char factoryMenuPosition = NOT_IN_MENU;
unsigned char findParrotFlag = 0; // flag set to 1 to trigger search for parrot, 0 if not

unsigned char rotaryEncoder; // variable to store value set by rotary encoder
unsigned char rotaryEncoderStr[MAX_ROTARY_ENCODER_DIGITS]; // equivalent string
unsigned char maxSpeed; // variable to store current max speed
unsigned char maxSpeedStr[MAX_ROTARY_ENCODER_DIGITS]; // equivalent string
unsigned char sigStrength;
unsigned char sigStrengthStr[MAX_SIGNAL_STRENGTH_DIGITS];

unsigned char FBJoystick;
unsigned char LRJoystick;

unsigned char Kp = 0;
unsigned char KpStr[];
unsigned char Ki = 0;
unsigned char KiStr[];
unsigned char Kd = 0;
unsigned char KdStr[];
unsigned char maxYawRate = 0;
unsigned char maxYawRateStr[];
unsigned char irSpe = 0;
unsigned char irSpeStr[];
unsigned char irFs = 0;
unsigned char irFsStr[];
unsigned char rfSpe = 0;
unsigned char rfSpeStr[];

unsigned char msg[MAX_LCD_CHARS];

// TODO: configure interrupts
// TODO: write to found and moving flags
// TODO: move functions into correct places, check variables and constants accounted for and in correct place
// TODO: figure out rotary encoder

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
	// RECEIVEserialPARSE();
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
	TRISD = 0; // LCD as output
}

// TODO: configure interrupts
void configInterrupts(void){
	INTCON3bits.INT1IP = 0; // INT1 external interrupt low priority
	INTCON2bits.INTEDG1 = 1; // interrupt on rising edge on RB1/INT1
	INTCON3bits.INT1IF = 0; // clear INT1 external interrupt
	INTCON3bits.INT1IE = 1; // enable INT1 external interrupt
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

void dispSigStrength(char* msg){
	strcpypgm2ram(msg,(const char*) "S=");
    disp_line(msg,UPPER_LINE,REPLACE);
    num2str(sigStrengthStr,sigStrength);
    disp_line(sigStrengthStr,UPPER_LINE,KEEP);
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

void askToSetToNewVal(unsigned char* variable,unsigned char* curVal,unsigned char* newVal){
	disp_line(variable,UPPER_LINE,REPLACE); // display variable name
    disp_line(curVal,UPPER_LINE,KEEP); // display current value on same line
    strcpypgm2ram(msg,(const rom char*) "Set to: ");
    disp_line(msg,UPPER_LINE,REPLACE); // ask user to set to new value set by rotary encoder
	disp_line(newVal,LOWER_LINE,KEEP); // display new value on same line
}

#pragma interrupt lowPriorityIsr
void lowPriorityIsr(void){
	// TODO: DISABLE INTERRUPTS OF SAME PRIORITY
	if(INTCON3bits.INT1IF){
        // External button interrupt. Priority: FACTORY >SELECT > RIGHT > LEFT
        if(FACTORY_BUT && userOrFactory == USER){ // if factory button pressed and in user mode
            userOrFactory = FACTORY; // switch to factory mode
        }
        else if(SELECT_BUT && userOrFactory == USER){ // if select button pressed and in user mode
            if(manualOrAuto == MANUAL){ // if in manual mode
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
            else if(manualOrAuto == AUTO){ // if in auto mode
                findParrotFlag = 1;
            }
        }
        else if(SELECT_BUT && userOrFactory == FACTORY){ // if select button pressed and in factory mode
            switch(factoryMenuPosition){
                case NOT_IN_MENU : // but not in menu when select button is pressed
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
                    irSpe = readRotaryEncoder(); // set IR samples per estimate to value established by rotary encoder
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
        else if(RIGHT_BUT && userOrFactory == USER && manualOrAuto == MANUAL && userMenuPosition < USER_MENU_END){ // if right button pressed and in user, manual mode and not at end of menu
            userMenuPosition++; // go to next menu item
        }
        else if(RIGHT_BUT && userOrFactory == FACTORY && factoryMenuPosition < FACTORY_MENU_END){ // if right button pressed and in factory mode and not at end of menu
            factoryMenuPosition++; // go to next menu item
        }
        else if(LEFT_BUT && userOrFactory == USER && manualOrAuto == MANUAL && userMenuPosition > NOT_IN_MENU){ // if left button pressed and in user, manual mode and in menu
            userMenuPosition--; // go to previous menu item
        }
        else if(LEFT_BUT && userOrFactory == USER && manualOrAuto == AUTO){ // if right button pressed and in user, auto mode
            manualOrAuto = MANUAL; // left button transitions to manual mode
        }
        else if(LEFT_BUT && userOrFactory == FACTORY && factoryMenuPosition > NOT_IN_MENU){ // if left button pressed and in factory mode and in menu
            factoryMenuPosition++; // go to next menu item
        }

        INTCON3bits.INT1IF = 0; // clear external interrupt flag
    }
}

void main(void){
    unsigned char msg[];
	while(1){
		if(manualOrAuto == MANUAL && userOrFactory == USER){
				
			if(toggleMotors){ // if motors need to be toggled
				if(motors){ // and they are on
					// SENDserialPACKAGE(MOTOR_STATE_DATA_ID,0); // turn them off
				}
				else{ // and they are off
					// SENDserialPACKAGE(MOTOR_STATE_DATA_ID,1); // turn them on
				}
				motors = !motors; // toggle motors flag
				toggleMotors = 0; // clear flag
			}
				
			if(interface1or2 == PRIMARY){
				
				FBJoystick = readFBJoystick(); // update fwd-back joystick value
				LRJoystick = readLRJoystick(); // update left-right joystick value
				
				// LCD displays in various menu positions
				switch(userMenuPosition){
					case NOT_IN_MENU : // if not in menu
						dispSigStrength(msg); // display signal strength on upper line
						strcpypgm2ram(msg,(const rom char*) "USER MANUAL MODE"); // display USER MANUAL MODE on lower line of LCD
						disp_line(msg,LOWER_LINE,REPLACE);
						break;
                        
					case SET_MOTOR_ON_OFF : // if in set motor on/off mode
						dispSigStrength(msg); // display signal strength on upper line
						if(motors){ // if motors are on
							strcpypgm2ram(msg,(const rom char*) "Turn motors off?"); // ask user if they want to turn them off
						}
						else{ // if motors are off
							strcpypgm2ram(msg,(const rom char*) "Turn motors on?"); // ask user if they want to turn them on
						}
						disp_line(msg,LOWER_LINE,REPLACE);
						break;
                        
					case SET_MAX_SPEED : // if in set max speed mode
						num2str(maxSpeedStr,maxSpeed); // convert max speed to a string
                        
						// display max speed and signal strength on upper line
                        dispSigStrength(msg);
                        strcpypgm2ram(msg,(const rom char*) " MaxSpd=");
                        disp_line(msg,UPPER_LINE,KEEP);
                        disp_line(maxSpeedStr,UPPER_LINE,KEEP);
                        
                        // display max speed value being set by rotary encoder on lower line
                 		getRotaryEncoderStr(); // get new max speed as a string from rotary encoder
                        strcpypgm2ram((unsigned char) msg,(const rom char*) "Set to: ");
                        disp_line(msg,LOWER_LINE,REPLACE);
                        disp_line(rotaryEncoderStr,LOWER_LINE,KEEP);
						break;
                        
					case SET_AUTO_MODE : // if in set auto mode
						dispSigStrength(msg); // display signal strength on upper line
						strcpypgm2ram(msg,(const rom char*) "Enter auto mode?"); // ask the user if they want to enter auto mode
						disp_line(msg,LOWER_LINE,REPLACE);
						break;
                        
					case USER_MENU_END : // if in switch to secondary UI mode
						dispSigStrength(msg); // display signal strength on upper line
						strcpypgm2ram(msg,(const rom char*) "Enter UI2?"); // ask the user if they want to enter secondary user interface
						disp_line(msg,LOWER_LINE,REPLACE);
                        
					default :
						break;
				}
				
				// LEDs (implemented without logic gates here...)
				commanderLEDs();
						 
				
			}
			
			// TODO: write manual mode secondary UI
			else if(interface1or2 == SECONDARY){
				
			}
		}

		else if(manualOrAuto == AUTO && userOrFactory == USER){
			if(interface1or2 == PRIMARY){
				// LCD display
				dispSigStrength(msg); // display signal strength on upper line
				strcpypgm2ram(msg,(const rom char*) "AUTO MODE"); // display AUTO MODE on lower line of LCD
				disp_line(msg,LOWER_LINE,REPLACE);
				
				// LEDs (implemented without logic gates here...)
				commanderLEDs();
			}
			
			// TODO: write auto mode secondary UI
			else if(interface1or2 == SECONDARY){
				
			}
			if(findParrotFlag){ // if findParrotFlag is set, commence search
				// SENDserialPACKAGE(START_DATA_ID,1);
			}
		}

		else if(userOrFactory == FACTORY){
			getRotaryEncoderStr(); // get new rotary encoder value as a string
			// LCD displays
			switch(factoryMenuPosition){
				case NOT_IN_MENU : // if FACTORY MODE but not in menu
					strcpypgm2ram(msg,(const rom char*) "FACTORY MODE"); // display FACTORY MODE on upper line of LCD
					disp_line(msg,UPPER_LINE,REPLACE);
					strcpypgm2ram(msg,(const rom char*) "Enter menu?"); // ask the user if they want to enter the menu
					disp_line(msg,LOWER_LINE,REPLACE);
					break;
                    
				// TODO: GET GAINS
				case SET_P_GAIN :					
					num2str(KpStr,Kp); // convert Kp to a string
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
					strcpypgm2ram(msg,(const rom char*) "SEL --> Manual"); // display FACTORY MODE on upper line of LCD
					disp_line(msg,UPPER_LINE,REPLACE);
					strcpypgm2ram(msg,(const rom char*) "NEXT --> Kp"); // ask the user if they want to enter the menu
					disp_line(msg,LOWER_LINE,REPLACE);
					break;
                    
				default :
					break;
			}
		}
	}
}