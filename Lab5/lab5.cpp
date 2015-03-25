/* Rev:$Revision: 1.1 $ */
/******************************************************************************
 * Copyright 1998-2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <dhcpclient.h>
#include <string.h>
#include "stepper.h"
#include "keypad.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "lcd.h"
#include "motorconstants.h"
#include "formdata.h"

const char *AppName = "put your name here";

extern "C"
{
   void UserMain( void *pd );
   void DisplayLameCounter( int sock, PCSTR url );
}

extern void RegisterPost();

FormData myData;
OS_SEM form_sem;
Keypad myKeypad;
Lcd myLCD;
Stepper myStepper(SM_MASTER_CHANNEL, SM_ACCEL_TABLE_SIZE);
#define MAX_COUNTER_BUFFER_LENGTH 100


void UserMain( void *pd )
{
	BYTE err = OS_NO_ERR;
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();

	eTPUInit();

	myLCD.Init(LCD_BOTH_SCR);
	myKeypad.Init();

	/* Initialise your formdata and stepper class here based on the output
	 * from the DIP switches.
	 */

	myStepper.Init(ECE315_ETPU_SM_FULL_STEP_MODE,
						SM_MAX_PERIOD,
						SM_INIT_SLEW_PERIOD);

	StartHTTP();
	EnableTaskMonitor();

	//Call a registration function for our Form code
	// so POST requests are handled properly.
	RegisterPost();

	myLCD.Clear(LCD_BOTH_SCR);
	myLCD.PrintString(LCD_UPPER_SCR, "Welcome to Lab 5 - ECE315");
	OSTimeDly(TICKS_PER_SECOND*1);

	while ( 1 )
	{

		myStepper.Step(100);// cw movement 100 steps = 1 rotation in full step mode
		OSTimeDly(TICKS_PER_SECOND*3);
		myStepper.Step(-100); // ccw movement 100 steps = 1 rotation in full step mode
		OSTimeDly(TICKS_PER_SECOND*3);
	}
}

/* Name: DisplayLameCounter
 * Description: Displays the number of times the page has been (re)loaded.
 * Meant to be called using <!-- FUNCTIONCALL DisplayLameCounter --> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: None
 */
void DisplayLameCounter( int sock, PCSTR url )
{
	static int form_counter = 0;
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if((sock > 0) && (url != NULL)) {
		iprintf(url);
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<H1>The page has been reloaded %d times. </H1>", form_counter );
		form_counter++;
		writestring(sock,(const char *) buffer);

	}
}




