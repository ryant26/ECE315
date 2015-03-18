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
#include "FormData.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "motorconstants.h"

const char *AppName = "RT CH";

extern "C"
{
	void UserMain( void *pd );
	void IRQIntInit(void);
	void SetIntc(int intc, long func, int vector, int level, int prio);
	void DisplayLameCounter( int sock, PCSTR url );
	void ValidateMinRPMImage (int sock, PCSTR url);
	void ValidateMaxRPMImage (int sock, PCSTR url);
	void ValidateRotationImage (int sock, PCSTR url);
	void ValidateDirectionImage (int sock, PCSTR url);
}

extern BYTE display_error(const char * info, BYTE error);
extern void RegisterPost();

#define MAX_COUNTER_BUFFER_LENGTH 100

char valid_imgstring[] = "<img source=\"hello\" width = 40 height = 40</img>";
char invalid_imgstring[] = "<img source=\"hello\" width = 40 height = 40</img>";



OS_SEM form_sem;
FormData myData(&form_sem);

void UserMain( void *pd )
{
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();

	//Call a registration function for our form code
	// so POST requests are handled properly.
	RegisterPost();
	display_error("Error initializing semaphor", OSSemInit(&form_sem, 1));

	// Insert your code that queries the DIP switches and
	// initialises the motor mode accordingly here.


	while ( 1 )
	{
		OSTimeDly(TICKS_PER_SECOND*100);
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

/*
 *
 */
void ValidateMinRPMImage (int sock, PCSTR url)
{
	if(myData.IsMinRPMValid()) {
		writestring(sock, (const char *)valid_imgstring);
	} else {
		writestring(sock, (const char *)invalid_imgstring);
	}
}

void ValidateMaxRPMImage (int sock, PCSTR url)
{
	if(myData.IsMaxRPMValid()) {
		writestring(sock, (const char *)valid_imgstring);
	} else {
		writestring(sock, (const char *)invalid_imgstring);
	}
}

void ValidateRotationImage (int sock, PCSTR url)
{
	if(myData.IsRotationsValid()) {
		writestring(sock, (const char *)valid_imgstring);
	} else {
		writestring(sock, (const char *)invalid_imgstring);
	}
}

void ValidateDirectionImage (int sock, PCSTR url)
{
	if(myData.IsDirectionValid()) {
		writestring(sock, (const char *)valid_imgstring);
	} else {
		writestring(sock, (const char *)invalid_imgstring);
	}
}




