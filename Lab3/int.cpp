/*
 * int.cpp
 *
 *	Created on: May 27th, 2013
 *	Author: 	Nancy Minderman
 *				nancy.minderman@ualberta.ca
 *
 */
#include <stdio.h>
#include <string.h>
#include <startnet.h>
#include <cfinter.h>
#include "sim5234.h"
#include "lcd.h"

#define WAIT_FOREVER			0

#define EPPAR_RISING_EDGE 	0x4
#define EPDDR_INPUT 		0xfff3
#define EPIER_ENABLED		0x2
#define EPFR_SET			0x2
#define NUM_ELEMENTS		10
#define NUM_EIR_FIELDS		13
#define EIR_OFFSET			19

#define UMASK_GRA 			0x10000000
#define INTCTRL0			0
#define PRIOR1				1
#define LVL4				4
#define LVL3				3
#define GRAVECTOR			32
#define IRQVECTOR			1
#define EPFR_SET			0x2
#define TCR_GTS_SET			0x1
#define TCR_GTS_CLEAR		0xfffe




extern "C" {
	// This routine helps you to initialise the interrupt controller properly
	void SetIntc( int intc, long func, int vector, int level, int prio );
}

extern Lcd myLCD;

vudword maskAndShift(unsigned int bit, vudword eimr){
	vudword mask = 0x1;
	mask = mask << bit;
	return (eimr & mask);
}

void printRegister(vudword value, char * name, unsigned char screen){
	char buffer [50];

	value = 1 && value;

	sprintf(buffer, "%s: %lu ", name, value);
	myLCD.PrintString(screen, buffer);
}

// Place your Interrupt querying code here for exercise 2
void QueryIntsFEC(void){

	char * regNames[NUM_EIR_FIELDS] = {"UN", "RL",
			"LC", "EBERR", "MII", "RXB", "RXF", "TXB", "TXF", "GRA", "BABT", "BABR", "HBERR"};

	myLCD.Clear(LCD_BOTH_SCR);

	for (int i = 0; i < NUM_EIR_FIELDS; i++){
		unsigned char screen;
		if (i <= 9){
			screen = LCD_UPPER_SCR;
		} else {
			screen = LCD_LOWER_SCR;
		}
		printRegister(maskAndShift(EIR_OFFSET + i, sim.fec.eimr), regNames[i], screen);
	}
}



// Place your INTERRUPT ISR code here for exercise 2
// Remember to signal to the processor to return to normal processing
// at the end of the interrupt
INTERRUPT(gra_isr, 0x2400) /* IRQ 4 Mask */
{
	sim.fec.eir |= UMASK_GRA;
	putleds(0xff);

}

INTERRUPT(irq1_isr, 0x2300) /* IRQ 3 Mask */
{
	sim.eport.epfr |= EPFR_SET;
	if (maskAndShift(0, sim.fec.tcr)){
		sim.fec.tcr &= TCR_GTS_CLEAR;
		putleds(0x00);
	} else {
		sim.fec.tcr |= TCR_GTS_SET;
	}

}



// Initialize the FEC interrupt mask register and interrupt controller
// for exercise 2 here.
// Use SetIntc to do this with the following values
// use level = 4 and prio = 1

void InitializeIntsFEC(void){
	sim.fec.eimr |=UMASK_GRA;
	SetIntc(INTCTRL0, (long)gra_isr, GRAVECTOR, LVL4, PRIOR1);
}

// Initialize the IRQ interrupt here. Use your lab 2 code as a guide
// The IRQ button is connected to edge port pin 1
// Use level = 3 and prio =1

void InitializeIntsIRQ1(void) {
		sim.eport.eppar |= EPPAR_RISING_EDGE;
		sim.eport.epddr &= EPDDR_INPUT;
		sim.eport.epier |= EPIER_ENABLED;

		SetIntc(INTCTRL0, (long)irq1_isr, IRQVECTOR, LVL3, PRIOR1);
}
