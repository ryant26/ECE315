#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <sim5234.h>
#include <cfinter.h>
#include "keypad.h"
#include "lcd.h"
#include <string.h>
#include "Cursor.h"

#define EPPAR_RISING_EDGE 	0x40
#define EPDDR_INPUT 		0xff3f
#define EPIER_ENABLED		0x8
#define EPFR_SET			0x8
#define NUM_ELEMENTS		10

#define LOWER_RIGHT 		79
#define UPPER_RIGHT			39
#define UPPER_LEFT			0
#define LOWER_LEFT			40
#define ONE_LINE			40

#define INTCTRL0			0
#define IRQVECTOR			3
#define INTLVL1				1
#define PRILVL1				1


extern "C" {
	void UserMain(void * pd);
	void IRQIntInit(void);
	void SetIntc(int intc, long func, int vector, int level, int prio);
}

const char * AppName="RT CH";

Keypad  myKeypad;
Lcd	myLCD;


/* Instantiate your Queue objects here */
OS_Q myQueue;
void * myQueueStorage[NUM_ELEMENTS];


void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;

	Cursor cursor (0, LCD_LOWER_SCR);
	InitializeStack();
	OSChangePrio(MAIN_PRIO);
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();

#ifndef _DEBUG
	EnableSmartTraps();
#endif

#ifdef _DEBUG
	InitializeNetworkGDB_and_Wait();
#endif

	iprintf("Application started: %s\n", AppName);

	myKeypad.Init();
	myLCD.Init(LCD_BOTH_SCR);
	myLCD.PrintString(LCD_UPPER_SCR, "ECE315 Lab #2 Winter 2015 ");
	OSTimeDly(TICKS_PER_SECOND*1);


	/* Initialize your queue and interrupt here */
	IRQIntInit();
	OSQInit(& myQueue, myQueueStorage, NUM_ELEMENTS);
	myLCD.Home(LCD_UPPER_SCR);
	while (1) {
		/* Insert your queue usage stuff */
		void * msg = OSQPend(&myQueue, 0, &err);
		display_error("Pend failed", err );
		iprintf((char*)msg);
		switch((*(char*)msg)) {
		case 'U':
			cursor.moveUp();
			iprintf("Up");
			break;
		case 'D':
			cursor.moveDown();
			iprintf("Down");
			break;
		case 'L':
			cursor.moveLeft();
			break;
		case 'R':
			cursor.moveRight();
			break;
		}
		myLCD.Clear(LCD_BOTH_SCR);
		myLCD.MoveCursor(cursor.currScreen, cursor.cursPos);
		myLCD.PrintChar(cursor.currScreen, 'A');



		/* You may also choose to do a quick poll of the Data Avail line
		 * of the encoder to convince yourself that the keypad encoder works.
		 */
	}
}

/* The INTERRUPT MACRO handles the house keeping for the vector table
 * and interrupt model.
 * Unfortunately the 0x2500 magic number must stay due to the MACRO syntax
 * The 0x2500 refers to the status register value that the microprocessor
 * will have during the interrupt.
 * Consult the NetBurner Runtime Libraries manual for information on this MACRO
 * Restrictions on what you can and cannot call inside an interrupt service routine
 * are listed there as well */

INTERRUPT(out_irq_pin_isr, 0x2500){
	sim.eport.epfr |= EPFR_SET;
	OSQPost(&myQueue, (void*)myKeypad.GetNewButtonString());
}

/* The registers that you need to initialise to get
 * interrupts functioning are:
 * sim.eport.eppar
 * sim.eport.epddr
 * sim.eport.epier
 *
 * You also need to call SetIntC with the correct parameters.
 *
 * void SetIntc(int intc, long func, int vector, int level, int prio)
 * int intc is the interrupt controller number for our interrupt
 * long func is our isr by name
 * int vector is the source vector number for our interrupt (i.e edge port module pin X)
 * this is in a table in the interrupt controller chapter in the ColdFire reference manual
 * int level use 1 here
 * int prio use 1 here as well
 * SetIntC is also documented in the NetBurner Runtime manual.
 *
 * Access the edge port registers by treating them as any variable
 * sim.eport.eppar = blah;
 *
 * The documentation on the edge port module will contain the information
 * on how to signal to the processor that it should return to normal processing.
 */


void IRQIntInit(void) {
	sim.eport.eppar |= EPPAR_RISING_EDGE;
	sim.eport.epddr &= EPDDR_INPUT;
	sim.eport.epier |= EPIER_ENABLED;

	SetIntc(INTCTRL0, (long)out_irq_pin_isr, IRQVECTOR, INTLVL1, PRILVL1);
}


