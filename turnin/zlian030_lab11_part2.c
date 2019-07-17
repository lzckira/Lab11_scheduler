  
/*	Author: zlian030
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "scheduler.h"
//#include "bit.h"
#include "io.c"
#include "io.h"
#include "keypad.h"
//#include "lcd_8bit_task.h"
//#include "queue.h"
//#include "seven_seg.h"
//#include "stack.h"
#include "timer.h"
//#include "usart.h"
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
unsigned char x;
const unsigned char str[] = "CS120B is Legend... wait for it DARY!";
enum LCDDisplay {scrolling};
unsigned char j = 0;

//Monitors button connected to PA0.
//When button is pressed, shared variable "pause" is toggled.
int LCDDisplaySM(int state){
	static unsigned char scroll = 16;
	static unsigned char disp = 0;
	static unsigned long crusor = 0;
	static unsigned long offset_trigger = 0;
	static unsigned long offset = 0;
	static unsigned long counter = 0;
	static unsigned int i = 16;
	unsigned long length = sizeof(str)/sizeof(str[0]);
	switch (state){
		case scrolling:
			LCD_ClearScreen();
            LCD_Cursor(16);
            if(counter < (length +15)){
				++counter;
			for(i= 16;i > scroll; --i){
				if ((crusor = (i-scroll-1 + offset)) >= (length -1)) {
					disp = 32;
				}
				else{
					disp = str[crusor];
				}
				LCD_Cursor(i);
				LCD_WriteData(disp);
			}
            if(scroll){
				--scroll;
				++offset_trigger;
			}
			if(offset_trigger >= 16){
				++offset;
			}
			}else{
				counter =0;
				scroll = 15;
				offset = 0;
				offset_trigger = 0;
			}
        default: 
			state = scrolling;
			break;
	}
	return state;
}
/*
//Enumeration of states.
enum toggleLED0_States { toggleLED0_wait, toggleLED0_blink};

int toggleLED0SMTick(int state){
		switch(state){
			case toggleLED0_wait: state = (!pause) ? toggleLED0_blink : toggleLED0_wait; break;
			case toggleLED0_blink: state = (pause) ? toggleLED0_wait : toggleLED0_blink; break;
			default: state = toggleLED0_wait; break;
		}
		switch(state){
			case toggleLED0_wait: break;
			case toggleLED0_blink:
				led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
				break;
		}
		return state;
}


enum toggleLED1_States {toggleLED1_wait, toggleLED1_blink};

int toggleLED1SMTick(int state){
		switch(state){
			case toggleLED1_wait: state = (!pause)? toggleLED1_blink: toggleLED1_wait; break;
			case toggleLED1_blink: state = (pause)? toggleLED1_wait:toggleLED1_blink; break;
			default: state = toggleLED1_wait; break;
		}
		switch(state){
			case toggleLED1_wait:break;
			case toggleLED1_blink:
				led1_output = (led1_output == 0x00)? 0x01: 0x00;
				break;
		}
		return state;
}

enum display_States {display_display};

int displaySMTick(int state){
		unsigned char output;
		switch(state){
			case display_display: 
					state = display_display;	
				break;
			default: state= display_display;break;
		}
		switch(state){
			case display_display:
				output = led0_output | led1_output << 1;
				break;
		}
		PORTB = output;scroll
		return state;
}

*/




int main(void) {
	
    /* Insert DDR and PORT initializations */
	/*unsigned char x;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC  = 0x0F;*/
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
    //Task 1 (pauseButtonToggleSM)
    
    task1.state = scrolling;//Task initial state
    task1.period = 100;//Task Period
    task1.elapsedTime = task1.period;//Task current elapsed time.
    task1.TickFct = &LCDDisplaySM;//Function pointer for the tick
    //Task 2 (toggleLED0SM)
    /*task2.state = toggleLED0_wait;//Task initial state
    task2.period = 1000;//Task Period
    task2.elapsedTime = task2.period;//Task current elapsed time.
    task2.TickFct = &toggleLED0SMTick;//Function pointer for the tick
    //Task 3 (toggleLED1SM)
    task3.state = toggleLED1_wait;//Task initial state
    task3.period = 200;//Task Period
    task3.elapsedTime = task3.period;//Task current elapsed time.
    task3.TickFct = &toggleLED1SMTick;//Function pointer for the tick
    //Task 4 (displaySM)
    task4.state = display_display;//Task initial state
    task4.period = 10;//Task Period
    task4.elapsedTime = task4.period;//Task current elapsed time.
    task4.TickFct = &displaySMTick;//Function pointer for the tick
    //Set the timer and turn it on
	unsigned long int GCD;
    */
    unsigned long GCD = tasks[0]->period;
    unsigned short i;//Scheduler for-loop iterator
    for ( i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}
    TimerSet(GCD);
    TimerOn();
    LCD_init();
    LCD_Cursor(1);
    while (1) {
		for (i = 0; i < numTasks; i++) { //Scheduler code
			if (tasks[i]->elapsedTime == tasks[i]->period) { //Task is ready to tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state); //set next state
				tasks[i]->elapsedTime = 0; //Reset the elapsed time for next tick;
			}
			tasks[i]->elapsedTime += GCD; 
			
		}
		while(!TimerFlag);
		TimerFlag = 0;
    }
    return 1;
}
