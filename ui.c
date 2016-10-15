#include "string.h"
#include "p18f4520.h"
// #include "SerialStuff.h"
#include "lcd.h"
#include "num2str.h"

#define SELECT_BUT	PORTBbits.RB0
#define BUTTON      PORTBbits.RB1
#define RIGHT_BUT	PORTBbits.RB2
#define LEFT_BUT	PORTBbits.RB3
#define	BACK_BUT	PORTBbits.RB4
#define FACTORY_BUT	PORTBbits.RB5
#define MANUAL_ENTRY	0 
#define ENTER_AUTO	1 
#define MOTORS_ON	2
#define MOTORS_OFF	3
#define CHANGE_MAX_SPEED	4
#define	SWITCH_INTERFACE	5
#define	AUTO_ENTRY	6
#define AUTO_FINDING	7
#define	FACTORY_ENTRY	8
#define	CHANGE_KP	9
#define	CHANGE_KI	10 
#define	CHANGE_KD	11
#define	CHANGE_MAX_SPEED_FACTORY	12
#define	CHANGE_YRM	13
#define	CHANGE_IR_SPE	14
#define	CHANGE_IR_SR	15
#define	CHANGE_RF_SPE	16
#define USER_MANUAL	0
#define USER_AUTO	1
#define FACTORY	2
#define PRIMARY 0
#define SECONDARY 1
#define FOUND_LED PORTCbits.RC0
#define MOVING_LED	PORTCbits.RC3
#define MAX_RSSI_DIGITS 3
#define MAX_ROTARY_ENCODER_DIGITS	3
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
void low_interrupt(void);
void high_interrupt(void);
void lowPriorityIsr(void);
void highPriorityIsr(void);

volatile unsigned char mode = USER_MANUAL; // user manual mode on startup
volatile unsigned char found = 0; // flag set to 1 when parrot found, 0 if not
volatile unsigned char moving = 0; // flag set to 1 when mobile robot is moving, 0 if stationary
volatile unsigned char interface = PRIMARY; // flag set to 1 in secondary UI, 0 in primary UI
volatile unsigned char LCDmode	= MANUAL_ENTRY; // LCD starts up in manual mode, not in menu

unsigned char rotaryEncoder; // variable to store value set by rotary encoder
unsigned char rotaryEncoderStr[MAX_ROTARY_ENCODER_DIGITS]; // equivalent string
volatile unsigned char maxSpd; // variable to store current max speed
unsigned char maxSpdStr[MAX_ROTARY_ENCODER_DIGITS]; // equivalent string
unsigned char rssi;
unsigned char rssiStr[MAX_RSSI_DIGITS];

unsigned char FBJoystick;
unsigned char LRJoystick;

// configurable parameters and corresponding strings
volatile unsigned char Kp = 0;
unsigned char KpStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char Ki = 0;
unsigned char KiStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char Kd = 0;
unsigned char KdStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char yrm = 0;
unsigned char yrmStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char irspe = 0;
unsigned char irspeStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char irsr = 0;
unsigned char irsrStr[MAX_ROTARY_ENCODER_DIGITS];
volatile unsigned char rfspe = 0;
unsigned char rfspeStr[MAX_ROTARY_ENCODER_DIGITS];

// TODO: wall message

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
    // TODO: transmit serial data package
	/* TODO:
     * receive serial data package:
     * - if m then 1, moving = 1; if m then 0, moving = 0
     * - if parrot found data ID then 1, found = 1; if parrot found data ID then 0, found = 0
     * - if rssi data ID, rssi = val and num2str(rssiStr,rssi)
     */
    return;
}

void configIOCommander(void){
	// PORTA - pretty sure you don't need to set TRIS for analogue inputs
//	TRISAbits.TRISA0 = 1; // fwd-back joystick as input
//	TRISAbits.TRISA1 = 1; // L-R joystick as input
//	TRISAbits.TRISA2 = 1; // rotary encoder as input
	
	// PORTB
	TRISBbits.TRISB0 = 1; // select button as input
	TRISBbits.TRISB1 = 1; // all buttons as input
	TRISBbits.TRISB2 = 1; // right button as input
    TRISBbits.TRISB3 = 1; // left button as input
    TRISBbits.TRISB4 = 1; // factory mode button as input
	
	// PORTC
	TRISCbits.TRISC0 = 0; // found LED as output
	TRISCbits.TRISC3 = 0; // moving LED as output
	TRISCbits.TRISC6 = 0; // EUSART Tx as output
	TRISCbits.TRISC7 = 1; // EUSART Rx as input
	
	// PORTD configured in LCD_init
}

// TODO: add serial interrupts
void configInterrupts(void){
	INTCONbits.GIE = 0; // disable interrupt response
    INTCONbits.PEIE = 0;
    RCONbits.IPEN = 1; // enable priority mode
    INTCON3bits.INT1IP = 0; // INT1 external interrupt low priority
	INTCON2bits.INTEDG1 = 1; // interrupt on rising edge on RB1/INT1
	INTCON3bits.INT1IF = 0; // clear INT1 external interrupt flag
	INTCON3bits.INT1IE = 1; // enable INT1 external interrupt
    INTCONbits.GIE = 1; // enable interrupt response
    INTCONbits.PEIE = 1;
}

void configAD(void){
	ADCON0 = 0; // AD off
	ADCON1 = 0b00001011; // AN3:0 Analog input; rest digital
	ADCON2 = 0; // Left justified to allow simple 8-bit read, 0 T_AD, Fosc/2
}

// TODO: test reading joysticks
unsigned char readFBJoystick(void){
	ADCON0 = 0b00000011; // Channel 0 (AN0/RA0), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
}

unsigned char readLRJoystick(void){
	ADCON0 = 0b00000111; // Channel 1 (AN1,RA1), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
}

// TODO: confirm how rotary encoder works
unsigned char readRotaryEncoder(void){
	ADCON0 = 0b00001011; // Channel 2 (AN2/RA0), GO = 1, ADON = 1
	while(PIR1bits.ADIF != 1){} // wait for conversion to complete
	return ADRESH; // read converted value
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

void askToSetToNewVal(void){
    putsLCD("Set to: ",LINE2); // display value to be set by rotary encoder on bottom line
	putsLCD(rotaryEncoderStr,1);
}

#pragma interrupt lowPriorityIsr
void lowPriorityIsr(void){
	// TODO: DISABLE INTERRUPTS OF SAME PRIORITY
	if(INTCON3bits.INT1IF){ // if button pressed
		if(interface == PRIMARY){ // and using primary interface
			// External button interrupt. Priority: FACTORY > SELECT > BACK > RIGHT > LEFT
			if(FACTORY_BUT && mode == USER_MANUAL){ // if factory button pressed and in user manual mode
				mode = FACTORY; // switch to factory mode
			}
			else if(SELECT_BUT){ // select button pressed
				if(mode == USER_MANUAL){ // and in manual mode
					switch(LCDmode){
						case MANUAL_ENTRY :	// if user enters menu
							LCDmode = ENTER_AUTO; // display 'ENTER AUTO MODE?'
							break;
						case ENTER_AUTO	: // if user enters auto mode
							LCDmode = AUTO_ENTRY; // display auto mode screen
							mode = USER_AUTO; // go into auto mode
							// TODO: send serial 'a' and 1
							break;
						case MOTORS_ON : // if user turns motors off
							LCDmode = MOTORS_OFF; // ask if user wants to turn motors on
							// TODO: send serial 'o' and 0
							break;
						case MOTORS_OFF : // if user turns motors on
							LCDmode = MOTORS_ON; // ask if user wants to turn motors off
							//TODO: send serial 'o' and 1 
							break;
						case CHANGE_MAX_SPEED : // if user sets new max speed
							maxSpd = rotaryEncoder; //  set max speed to rotary encoder value
							// TODO: send serial 's' and max speed
							break;
						case SWITCH_INTERFACE : // if user selects secondary interface
							interface = SECONDARY; // switch interface
							cmd2LCD(CLR_DISP); // clear LCD display
							break;
						default :
							break;
					}
				}
				
				else if(mode == USER_AUTO && LCDmode == AUTO_ENTRY){ // if in auto mode and haven't started finding and select button pressed
					// TODO: send start finding parrot
				}
				
				else if(mode == FACTORY){ // if in factory mode
					switch(LCDmode){
						case FACTORY_ENTRY :
							LCDmode = CHANGE_KP; // if user enters factory menu, go to change Kp screen
							break;
						case CHANGE_KP :
							Kp = rotaryEncoder; // set Kp to rotary encoder value
							// TODO: send serial 'p' and Kp
							break;
						case CHANGE_KI :
							Ki = rotaryEncoder; // set Ki to rotary encoder value
							// TODO: send serial 'i' and Ki
							break;
						case CHANGE_KD :
							Kd = rotaryEncoder; // set Kd to rotary encoder value
							// TODO: send serial 'd' and Kd
							break;
						case CHANGE_MAX_SPEED_FACTORY :
                            maxSpd = rotaryEncoder; // set max speed to rotary encoder value
							// TODO: send serial 's' and max speed
							break;
						case CHANGE_YRM :
							yrm = rotaryEncoder; // set max yaw rate to rotary encoder value
							// TODO: send serial 'y' and yrm
							break;
						case CHANGE_IR_SPE :
							irspe = rotaryEncoder; // set IR samples per estimate to rotary encoder value
							// TODO: send serial 'g' and irspe
							break;
						case CHANGE_IR_SR :
							irsr = rotaryEncoder; // set IR sample rate to rotary encoder value
							// TODO: send serial 'r' and irsr
							break;
						case CHANGE_RF_SPE :
							rfspe = rotaryEncoder; // set RF samples per estimate to rotary encoder value
							// TODO: send serial 'f' and rfspe
							break;
						default :
							break;
					}
				}
			}
			else if(BACK_BUT){ // back button pressed
				if(mode == USER_AUTO || mode == FACTORY){ // if in auto mode or factory mode, back button takes user back to manual mode
					// TODO: send serial 'a' and 0
				}
				LCDmode = MANUAL_ENTRY;
				mode = USER_MANUAL;
			}
			else if(RIGHT_BUT){ // right button pressed
				if(mode == USER_MANUAL){ // user manual mode
					if(LCDmode != MANUAL_ENTRY && LCDmode != SWITCH_INTERFACE){ // if not outside or at end of menu, go to next menu item
						LCDmode++;
					}
					else if(LCDmode == SWITCH_INTERFACE){ // if at end of menu, go to start of menu
						LCDmode = ENTER_AUTO; 
					}
				}
				// right button should do nothing in auto mode
				else if(mode == FACTORY){ // factory mode
					if(LCDmode != FACTORY_ENTRY && LCDmode != CHANGE_RF_SPE){ // if not outside or at end of menu, go to next menu item
						LCDmode++;
					}
					else if(LCDmode == CHANGE_RF_SPE){ // if at end of menu, go to start of menu
						LCDmode = CHANGE_KP;
					}
				}
			}
			else if(LEFT_BUT){ // left button pressed
				if(mode == USER_MANUAL){ // user manual mode
					if(LCDmode != MANUAL_ENTRY && LCDmode != ENTER_AUTO){ // if not outside or at start of menu, go to previous menu item
						LCDmode--;
					}
					else if(LCDmode == ENTER_AUTO){ // if at start of menu, go to end of menu
						LCDmode = SWITCH_INTERFACE; 
					}
				}
				// left button should do nothing in auto mode
				else if(mode == FACTORY){ // factory mode
					if(LCDmode != FACTORY_ENTRY && LCDmode != CHANGE_KP){ // if not outside or at start of menu, go to previous menu item
						LCDmode--;
					}
					else if(LCDmode == CHANGE_KP){ // if at start of menu, go to end of menu
						LCDmode = CHANGE_RF_SPE;
					}
				}
			}
		}
		INTCON3bits.INT1IF = 0; // clear external interrupt flag
	}
    return;
}

void main(void){
    unsigned char msg[];
    configIOCommander();
    configAD();
    configInterrupts();
    LCD_init(); // turn LCD on
	while(1){		
		if(interface == PRIMARY){
			
			FBJoystick = readFBJoystick(); // update fwd-back joystick value
			LRJoystick = readLRJoystick(); // update left-right joystick value
			// TODO: get rotary encoder value
			// rotaryEncoder = _______
			num2str(rotaryEncoderStr,rotaryEncoder); // convert the value set by the rotary encoder to a string
			
			// LEDs (implemented without logic gates here...)
			commanderLEDs();
			
			// LCD displays
			if(mode == USER_MANUAL){ // manual mode
				putsLCD(rssiStr,LINE1); // display rssi to top left of LCD
				putsLCD("MANUAL",1); // tell user they are in manual mode in top right
				switch(LCDmode){
					case MANUAL_ENTRY :	// outside menu
						putsLCD("Enter menu?",LINE2); // ask user if they want to enter menu on lower line
						break;
					case ENTER_AUTO	: // asking to enter auto mode
						putsLCD("Enter auto mode?",LINE2); // ask user if they want to enter auto mode on lower line
						break;
					case MOTORS_ON : // if motors are on
						putsLCD("Turn motors off?",LINE2); // ask user if they want to turn them off on lower line
						break;
					case MOTORS_OFF : // if motors are off
						putsLCD("Turn motors on?",LINE2); // ask user if they want to turn them on on lower line
						break;
					case CHANGE_MAX_SPEED : // if user sets new max speed
						putsLCD("Max:",LINE2); // diplay current max speed on lower line
						putsLCD(maxSpdStr,0);
						putsLCD("New:",1); // adjacent, display value to given by rotary encoder
						putsLCD(rotaryEncoderStr,0);
						break;
					case SWITCH_INTERFACE : // ask user if they want to switch UI
						putsLCD("Switch interface?",LINE2);
						break;
					default :
						break;
				}
			}
		
			else if(mode == USER_AUTO){ // if in auto mode
				putsLCD("AUTO",LINE1); // tell the user they are in auto mode
				if(LCDmode == AUTO_ENTRY){ // if finding parrot hasn't begun
					putsLCD("SEL: Find parrot",LINE2); // tell user that pressing select commences search
				}
				else if(LCDmode == AUTO_FINDING){ // if finding parrot has begun
					putsLCD(rssiStr,LINE2); // display rssi to bottom left of LCD
					if(found){	// if parrot found, tell user
						putsLCD("Found!",1); // tell user
					}
					else{ // if parrot not found
						putsLCD("Finding...",1); // tell user it is being found
					}
				}
			}
			
			else if(mode == FACTORY){ // if in factory mode
				switch(LCDmode){
					case FACTORY_ENTRY : // if outside factory mode menu
						putsLCD("FACTORY",LINE1); // tell user they are in factory mode
						putsLCD("Enter menu?",LINE2); // ask user if they want to enter menu on lower line
						break;
					case CHANGE_KP :
						putsLCD("Kp:",LINE1); // display current Kp on top line
						putsLCD(KpStr,1);
						askToSetToNewVal(); // display Kp to be set by rotary encoder on bottom line
						break;
					case CHANGE_KI :
						putsLCD("Ki:",LINE1); // display current Ki on top line
						putsLCD(KiStr,1);
						askToSetToNewVal(); // display Ki to be set by rotary encoder on bottom line
						break;
					case CHANGE_KD :
						putsLCD("Kd:",LINE1); // display current Kd on top line
						putsLCD(KdStr,1);
						askToSetToNewVal(); // display Kd to be set by rotary encoder on bottom line
						break;
					case CHANGE_MAX_SPEED_FACTORY :
						putsLCD("Max Speed:",LINE1); // display current max speed on top line
						putsLCD(maxSpdStr,1);
						askToSetToNewVal(); // display max speed to be set by rotary encoder on bottom line
						break;
					case CHANGE_YRM :
						putsLCD("Max yaw rate:",LINE1); // display current max yaw rate on top line
						putsLCD(yrmStr,1);
						askToSetToNewVal(); // display max yaw rate to be set by rotary encoder on bottom line
						break;
					case CHANGE_IR_SPE :
						putsLCD("IR SPE:",LINE1); // display current IR samples per estimate on top line
						putsLCD(irspeStr,1);
						askToSetToNewVal(); // display IR samples per estimate to be set by rotary encoder on bottom line
						break;
					case CHANGE_IR_SR :
						putsLCD("IR SR:",LINE1); // display current IR sample rate on top line
						putsLCD(irsrStr,1);
						askToSetToNewVal(); // display IR sample rate to be set by rotary encoder on bottom line
						break;
					case CHANGE_RF_SPE :
						putsLCD("RF SPE:",LINE1); // display current RF samples per estimate on top line
						putsLCD(rfspeStr,1);
						askToSetToNewVal(); // display RF samples per estimate to be set by rotary encoder on bottom line
						break;
					default :
						break;
				}
			}
		}
		// TODO: write manual mode secondary UI
		else if(interface == SECONDARY){
			
		}
        
        // TODO: calculate speed right and speed left
        // TODO: send speed right ('b' then value)
        // TODO: send speed left ('c' then value)
	}
}