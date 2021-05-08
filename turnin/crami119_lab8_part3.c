/*	Author: Cruz Ramirez
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab 8  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *	Speaker plays a short melody
 *
 *
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL> https://www.youtube.com/watch?v=dFpDTkjC7XU
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer code

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;



void TimerOn() {

        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){

        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

//END OF TIMER CODE


//SPEAKER CODE


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

//END OF SPEAKER CODE

unsigned char inv;

unsigned char curr_note = 0;

double FRQ[17] = {587.33, 440.00, 587.33, 880.00, 783.99, 698.46, 659.25, 523.25, 0, 523.25, 440.00, 523.25, 698.46, 659.25, 523.25, 587.33, 698.46};

enum NOTE_States{NOTE_Wait, NOTE_Note, NOTE_End}NOTE_State;

void TickFct_NOTE(){

	switch(NOTE_State){ //state transitions

		case NOTE_Wait:
			if(inv == 0x01){
				NOTE_State = NOTE_Note;
			}
			else{
				NOTE_State = NOTE_Wait;
			}
		break;

		case NOTE_Note:
			if(curr_note == 17){
				curr_note = 0;
				NOTE_State = NOTE_End;
			}
			else{
				++curr_note;
				NOTE_State = NOTE_Note;
			}
		break;

		case NOTE_End:
			if(inv == 0x00){
				NOTE_State = NOTE_Wait;
			}
			else{
				NOTE_State = NOTE_End;
			}
		break;


		default:
		break;
	}

	switch(NOTE_State){ //state actions

		case NOTE_Note:
			set_PWM(FRQ[curr_note]);	
		break;
		
		case NOTE_End:
			set_PWM(0);
		break;


		case NOTE_Wait:
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
	TimerSet(300);
	TimerOn();

	//intial state
	NOTE_State = NOTE_Wait;
    while (1) {

	inv = ~PINA; //get inverse value for function
	
       	while(!TimerFlag);
	TimerFlag = 0;
	 
	TickFct_NOTE();

    }
    return 0;
}
