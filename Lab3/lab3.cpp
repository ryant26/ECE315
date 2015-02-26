#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <utils.h>
#include <ucos.h>
#include <cfinter.h>
#include <ip.h>
#include <tcp.h>
#include <udp.h>
#include <multihome.h>
#include <nbtime.h>
#include <rtc.h>
#include <buffers.h>
#include "sim5234.h"
#include "services_udp.h"
#include "lcd.h"
#include "ethervars.h"

#define	WAIT_FOREVER 0
#define IBUFF_SIZE_LINE 		40


extern "C" {
	void UserMain(void * pd);
	void GraMain(void *);
	void StartGracefulStopTask(void);
	void InitLEDs(void);
}

extern void QueryIntsFEC(void);
extern void InitializeIntsFEC(void);
extern void InitializeIntsIRQ1(void);
extern void RequestGracefulStop(void);
extern void ClearGracefulStop(void);

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available
   See services_udp.h for more info on the server tasks.
*/

#define GRA_PRIO 				MAIN_PRIO + 1


asm( " .align 4 " );
DWORD GraMainStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );


const char * AppName="Put your name here";

Lcd myLCD;

/****************************************/
void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;

    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    StartEchoServer();
    StartChargenServer();
    EnableTaskMonitor();


    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started\n");

    myLCD.Init(LCD_BOTH_SCR);
    myLCD.PrintString(LCD_UPPER_SCR, "Welcome to ECE315-Winter 2015");
    OSTimeDly(TICKS_PER_SECOND*1.5);

    StartGracefulStopTask();

    QueryIntsFEC();
    OSTimeDly(TICKS_PER_SECOND*10);

    InitializeIntsFEC();
    OSTimeDly(1);

    QueryIntsFEC();
    OSTimeDly(TICKS_PER_SECOND*10);

    InitializeIntsIRQ1();

    myLCD.Clear(LCD_BOTH_SCR);

    while (1) {
    	OSTimeDly(TICKS_PER_SECOND*1);

	}
}


/* Name: StartGracefulStopTask
 * Description: Creates the Task responsible for generating and
 * clearing the graceful stop condition.
 * Inputs: none
 * Outputs: none
 */
void StartGracefulStopTask(void) {
	BYTE err = OS_NO_ERR;

	/* start up the task that lights up LEDS */
	err = display_error("StartGracefulStopTask", OSTaskCreatewName(	GraMain,
					(void *)NULL,
				 	(void *) &GraMainStk[USER_TASK_STK_SIZE],
				 	(void *) &GraMainStk[0],
				 	GRA_PRIO, "Gra Task" ));
}

/* Name: GraMain
 * Description: This task contains the graceful stop task code.
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	GraMain( void * pd) {
	BYTE err = OS_NO_ERR;
	while (1) {
		OSTimeDly(TICKS_PER_SECOND*1);
	}
}

