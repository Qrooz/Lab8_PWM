/*	Author: Cruz Ramirez
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab 8  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *	Speaker can play eight different notes and can scale up and down even when off.
 *
 *
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL> https://www.youtube.com/watch?v=ZflkQbXO6Z0
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {

	static double current_frequency;

	if(frequency != current_frequency){

		if(!frequency){ TCCR3B &= 0x08;}
		else{TCCR3B |= 0x03; }

		if(frequency < 0.954){OCR3A = 0xFFFF; }
		else if(frequency > 31250){OCR3A = 0x0000;}

		else {OCR3A = (short)(8000000/(128 * frequency)) -1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char curr_note = 0; //number representation of current note global variable
unsigned char inv = 0; //inverse of PINA 


enum CNT_States{CNT_Start, CNT_Wait, CNT_ScaleUp, CNT_ScaleDown}CNT_State;

void TickFct_CNT(){

	switch(CNT_State){ //state transitions for count state machine
	
		case CNT_Start:
			CNT_State = CNT_Wait;
		break;

		case CNT_Wait:
			if(inv == 0x01){
				CNT_State = CNT_ScaleUp;
				if(curr_note < 7){
				++curr_note;
				}
			}
			else if(inv == 0x02){
				CNT_State = CNT_ScaleDown;
				if(curr_note > 0){
                        	--curr_note;
                        	}
			}
			else{
				CNT_State = CNT_Wait;
			}	
		break;

		case CNT_ScaleUp:
			if(inv == 0x00){
			CNT_State = CNT_Wait;
			}
			else{
			CNT_State = CNT_ScaleUp;
			}
		break;

		case CNT_ScaleDown:
			if(inv == 0x00){
			CNT_State = CNT_Wait;
			}
			else{
			CNT_State = CNT_ScaleDown;
			}
		break;

	}

	switch(CNT_State){//state actions for count state
	
		case CNT_Start:
			curr_note = 0;
		break;

		case CNT_Wait:
		case CNT_ScaleUp: //actions taken care of in mealy action to account for holding the button
		case CNT_ScaleDown:
		default:
		break;

	}
}

enum MODE_States{MODE_Play1, MODE_Play2, MODE_Off, MODE_WaitOff}MODE_State;

void TickFct_MODE(){

	switch(MODE_State){ //state transitions for mode state machine
	
		case MODE_Play1:
			if(inv == 0x04){
				MODE_State = MODE_Off;
			}
			else{
				MODE_State = MODE_Play1;
			}
		break;

		case MODE_Off:
			if(inv == 0x00){
				MODE_State = MODE_WaitOff;
			}
			else{
				MODE_State = MODE_Off;
			}
		break;

		case MODE_WaitOff:
			if(inv == 0x04){
				MODE_State = MODE_Play2;
			}
			else{
				MODE_State = MODE_WaitOff;
			}
		break;

		case MODE_Play2:
			if(inv == 0x00){
				MODE_State = MODE_Play1;
			}
			else{
				MODE_State = MODE_Play2;
			}
		break;
	}

	switch(MODE_State){ //state actions for Mode state machine
		
		case MODE_Play1:
			if(curr_note == 0){
				set_PWM(261.63); //freq for c4
			}
			else if(curr_note == 1){
				set_PWM(293.66); //freq for D4
			}
			else if(curr_note == 2){
                                set_PWM(329.63); //freq for E4
                        }
			else if(curr_note == 3){
                                set_PWM(349.23); //freq for F4
                        }
			else if(curr_note == 4){
                                set_PWM(392.00); //freq for G4
                        }
			else if(curr_note == 5){
                                set_PWM(440.00); //freq for A4
                        }
			else if(curr_note == 6){
                                set_PWM(493.88); //freq for B4
                        }
			else if(curr_note == 7){
                                set_PWM(523.25); //freq for C5
                        }
		break;

		case MODE_Play2:
			if(curr_note == 0){
                                set_PWM(261.63); //freq for c4
                        }
                        else if(curr_note == 1){
                                set_PWM(293.66); //freq for D4
                        }
                        else if(curr_note == 2){
                                set_PWM(329.63); //freq for E4
                        }
                        else if(curr_note == 3){
                                set_PWM(349.23); //freq for F4
                        }
                        else if(curr_note == 4){
                                set_PWM(392.00); //freq for G4
                        }
                        else if(curr_note == 5){
                                set_PWM(440.00); //freq for A4
                        }
                        else if(curr_note == 6){
                                set_PWM(493.88); //freq for B4
                        }
                        else if(curr_note == 7){
                                set_PWM(523.25); //freq for C5
                        }
                break;

		case MODE_Off:
			set_PWM(0);
		break;

		case MODE_WaitOff:
		default:
		break;
	}

}




int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
	PWM_on();
    while (1) {

	inv = ~PINA; //get inverse value to be used with both functions

	TickFct_CNT();
	TickFct_MODE();
    }
    return 0;
}
