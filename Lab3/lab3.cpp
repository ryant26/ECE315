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
#define LCD_SECOND_LINE			40
#define BUFFER_SIZE				50

extern "C" {
	void UserMain(void * pd);
	void GraMain(void *);
	void NetStatsMain(void *);
	void StartGracefulStopTask(void);
	void StartNetworStatisticsTask(void);
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
#define NETSTATS_PRIO 			MAIN_PRIO + 5

asm( " .align 4 " );
DWORD GraMainStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD NetStatsSTK[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

const char * AppName="CH RT";

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

    StartNetworStatisticsTask();

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

/* Name: StartNetworStatisticsTask
 * Description: Creates the Task responsible printing network
 * usage statistics to the LCD screen.
 * Inputs: none
 * Outputs: none
 */
void StartNetworStatisticsTask(void) {
	BYTE err = OS_NO_ERR;

	/* start up the task that lights up LEDS */
	err = display_error("StartNetworkStatisticsTask", OSTaskCreatewName(	NetStatsMain,
					(void *)NULL,
				 	(void *) &NetStatsSTK[USER_TASK_STK_SIZE],
				 	(void *) &NetStatsSTK[0],
				 	NETSTATS_PRIO, "NetStats Task" ));
}

/* Name: NetStatsMain
 * Description: This task contains the network statistics printing code.
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	NetStatsMain( void * pd) {
	BYTE err = OS_NO_ERR;
	while (1) {
		vudword totalPackets = sim.fec_rmon_t.packets;
		vudword broadcastPackets = sim.fec_rmon_t.bc_pkt;
		vudword multicastPackets = sim.fec_rmon_t.mc_pkt;
		vudword unicastPackets = totalPackets - broadcastPackets - multicastPackets;

		char printBuff[BUFFER_SIZE];


		myLCD.Clear(LCD_BOTH_SCR);
		myLCD.Home(LCD_UPPER_SCR);

		snprintf(printBuff, BUFFER_SIZE,"Total Packets: %lu ", totalPackets);
		myLCD.PrintString(LCD_UPPER_SCR, printBuff);

		myLCD.MoveCursor(LCD_UPPER_SCR, LCD_SECOND_LINE);
		snprintf(printBuff, BUFFER_SIZE, "Broadcast Packets: %lu ", broadcastPackets);
		myLCD.PrintString(LCD_UPPER_SCR, printBuff);

		myLCD.Home(LCD_LOWER_SCR);
		snprintf(printBuff, BUFFER_SIZE,"Multicast Packets: %lu ", multicastPackets);
		myLCD.PrintString(LCD_LOWER_SCR, printBuff);

		myLCD.MoveCursor(LCD_LOWER_SCR, LCD_SECOND_LINE);
		snprintf(printBuff, BUFFER_SIZE, "Unicast Packets: %lu ", unicastPackets);
		myLCD.PrintString(LCD_LOWER_SCR, printBuff);

		OSTimeDly(TICKS_PER_SECOND*1);
	}
}
