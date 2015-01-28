/*
 * Nancy Minderman
 * nem@ece.ualberta.ca
 * May 9, 2013
 * Tutorial code for ECE 315 Winter 2015
 * Place all your new code in this file.
 */

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <system.h>


/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define LED_PRIO MAIN_PRIO + 1
#define SEG_PRIO MAIN_PRIO + 2

/* These are bit field values corresponding to the least and most
 * significant bit of the LEDS just below the 7 segment display on the
 * v 1.08 carrier board */
#define LED1 		0x01
#define LED2		0x02
#define LED3		0x04
#define LED4		0x08
#define LED5		0x10
#define LED6		0x20
#define LED7		0x40
#define LED8 		0x80
#define LEDS_ON 	0xff
#define LEDS_OFF 	0x00

extern "C" {
void UserMain(void * pd);
void InitLEDs (void);
void LEDsMain(void *);
void StartLEDs(void);
void InitSEG(void);
void SEGMain(void *);
void StartSEG(void);
}

const char * AppName="Lab 0 CH, RT 1";

asm( " .align 4 " );
DWORD LEDMainStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD SEGMainStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

/* Name: UserMain
 * Description: This is the entry point for the operating system
 * It spawns all new tasks. Currently does nothing but sleep.
 * Inputs: void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    StartLEDs();
    StartSEG();
    EnableTaskMonitor();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started: %s\n", AppName);
    while (1) {
        OSTimeDly(20);
    }
}

/* Name: StartLEDs
 * Description: Initialises the LED hardware and spawns a task that
 * does stuff with the LEDS. The LEDsMain function contains the task
 * code.
 * Inputs: none
 * Outputs: none
 */
void StartLEDs(void) {
	BYTE err = OS_NO_ERR;

	/* hardware init here if needed */
	InitLEDs();

	/* start up the task that lights up LEDS */
	err = OSTaskCreatewName(	LEDsMain,
					(void *)NULL,
				 	(void *) &LEDMainStk[USER_TASK_STK_SIZE],
				 	(void *) &LEDMainStk[0],
				 	LED_PRIO, "LED Task" );
	if(err) {
		iprintf("Task Creation Error in StartLEDs\n");
	}
}

/* Name: LEDsMain
 * Description: This function contains the LED task code. To change the
 * way the LEDS light up change the code in the while loop.
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	LEDsMain( void * pd) {
	unsigned short led = LED1;

	/* Place your code modifications inside the while to complete the small coding assignment */
	while (1) {

		/* generate a simple trailing pattern starting at LED1 */
		putleds(getdipsw());

		/*led <<= 1;
		if (led > LED8) {
			led = LED1;
		};/*

		/* delay this task for at least 200 ms before executing again */
		OSTimeDly(TICKS_PER_SECOND/5);
	}
}

/* Name: StartSEG
 * Description: This function initialises the seven segment hardware and
 * spawns a task to do stuff with the seven segment display. The SegMain
 * function contains the task code.
 * Inputs: none
 * Outputs: none
 */
void StartSEG(void) {
	BYTE err = OS_NO_ERR;

	/* hardware init here if needed */
	InitSEG();

	/* start up the task that lights up 7 Segment Displays */
	err = OSTaskCreatewName(	SEGMain,
					(void *)NULL,
				 	(void *) &SEGMainStk[USER_TASK_STK_SIZE],
				 	(void *) &SEGMainStk[0],
				 	SEG_PRIO, "7 Segment Task" );
	if(err) {
		iprintf("Task Creation Error in StartSEG\n");
	}
}

/* Name: SEGMain
 * Description: This function contains the code that writes to the
 * seven segment displays. To change the values that are displayed,
 * change the code in the while loop.
 * Inputs: void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void SEGMain( void * pd) {

	unsigned short seg_values[] =
		{0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
				0x7777, 0x8888, 0x9999
		};
	unsigned short *  ptr_seg;
	int i = 0;
	while(1) {

		if (i > 9){
			i = 0;
		}
		/* each of the seven segment displays takes a BCD number */
		putdisp(seg_values[i]);

		/* pattern should be 0000, 1111, 2222, etc and then reset */
		/* modify the putdisp call to use the ptr_seg variable and/or the
		 * seg_values array to display an incrementing count.
		 * Either/both will work.
		 */
		i++;
		/* sleep for about 2 seconds before executing again */
		OSTimeDly(TICKS_PER_SECOND*2);
	}
}

/* Name: InitLEDs
 * Description: This function does all LED hardware initializations
 * and sets all 8 LEDS to be initially off.
 * Inputs: none
 * Outputs: none
 */
void InitLEDs(void) {
	putleds(LEDS_OFF);
}

/* Name: InitSEG
 * Description: This function does all seven segment hardware
 * initializations and sets each of the 4 seven segment displays to 0.
 * Inputs: none
 * Outputs: none
 */
void InitSEG(void) {
	putdisp(0);
}
