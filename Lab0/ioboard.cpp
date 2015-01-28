/* $Revision: 1.3 $ */
/*******************************************************************************
*                                                                              *
*   © 2000-2007 NetBurner, Inc.  ALL RIGHTS RESERVED                           *
*                                                                              *
*   Permission is hereby granted to purchasers of NetBurner hardware to use    *
*   or modify this computer program for any use as long as the resultant       *
*   program is only executed on NetBurner provided hardware.                   *
*                                                                              *
*   No other rights to use this program or its derivatives in part or in       *
*   whole are granted.                                                         *
*                                                                              *
*   It may be possible to license this or other NetBurner software for use on  *
*   non-NetBurner hardware. Please contact sales@netburner.com.                *
*                                                                              *
*   NetBurner makes no representation or warranties with respect to the        *
*   performance of this computer program, and specifically disclaims any       *
*   responsibility for any damages, special or consequential, connected with   *
*   the use of this program.                                                   *
*                                                                              *
*   NetBurner, Inc.                                                            *
*   5405 Morehouse Drive                                                       *
*   San Diego, CA  92121                                                       *
*                                                                              *
*   Information available at:  http://www.netburner.com                        *
*   Support availible at:      http://support.netburner.com                    *
*                                                                              *
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Note on using the MOD5234 with the MOD-DEV-70 development board - the     //
//  following pins are used by this driver file (calling the getdipsw() and   //
//  putleds() functions may inadvertently affect your custom application if   //
//  you plan on using the below mentioned pins):                              //
//                                                                            //
//     DIP Switches:  J2-6,  J2-7,  J2-8,  J2-9,  J2-10, J2-11, J2-12, J2-13  //
//     LEDs:          J2-15, J2-16, J2-17, J2-18, J2-19, J2-20, J2-23, J2-24  //
//                                                                            //
//  With the exception of J2-23 and J2-24, the eTPU pins are initialized only //
//  once as GPIO when one of the functions - getdipsw() or putleds() - is     //
//  called for the first time.  Pins J2-23 and J2-24 are primarily UART 1     //
//  RTS/CTS pins, and are initialized as GPIO every time putleds() is called. //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "predef.h"
#include <stdio.h>
#include "sim5234.h"                         // On-chip register definitions
#include "includes.h"
#include "cfinter.h"
#include "constants.h"
#include "iosys.h"
#include "bsp.h"
#include "system.h"
#include "utils.h"
#include "ETPUinit.h"
#include "pins.h"


#define BASEADDRESS   ( 0xA0000000 )


static BOOL bInit;          // Make sure initialization is performed
static volatile PBYTE pLed;
static BOOL bInitModDev70;  // Check for first detection of a MOD-DEV-70 board
static BOOL bIsModDev70;    // Check for usage on a MOD-DEV-70 development board
static BYTE u8_dipsw;       // Stores state of DIPs until function returns


////////////////////////////////////////////////////////////////////////////////
// InitChipBoardSelect
//
void InitChipBoardSelect( void )
{
   sim.cs[3].csar = ( BASEADDRESS >> 16 );
   sim.cs[3].cscr = 0x2140;                  // 0010 0001 0100 0000
   sim.cs[3].csmr = 0x001F0001;
   pLed = ( PBYTE ) BASEADDRESS;
   pLed[1] = 0xFF;
   pLed[2] = 0xFF;
   bInit = TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// ModDev70Detect - This function tests to see whether the MOD-DEV-70 carrier
// board is being used, otherwise a MOD-DEV-100 is assumed by default.  This is
// done by pulling a designated signal pin low and then reading back its state.
// Since the LEDs on the MOD-DEV-70 will naturally pull this pin high, it will
// read back in a high state.  The MOD-DEV-100 leaves this pin unconnected, so
// it will remain low and read back in a low state.
//
BOOL ModDev70Detect( void )
{
   if ( !bInitModDev70 )
   {
      sim.gpio.par_uart &= ~0x0030;  // Configure pin J2-23 for GPIO function
      sim.gpio.pclrr_uartl = ~0x40;  // Set J2-23 low
      sim.gpio.pddr_uartl |= 0x40;   // Configure J2-23 as an output

      //
      // Short delay to ensure J2-23 is pulled low.
      //
      for ( volatile int i = 0; i < 4000000; i++ ) asm(" nop");

      sim.gpio.pddr_uartl &= ~0x40;  // Configure J2-23 as an input

      asm(" nop");                   // Short delay to allow pin to pull up
      asm(" nop");

      //
      // If J2-23 was pulled up, then the carrier board mounted onto is a
      // MOD-DEV-70.
      //
      if ( sim.gpio.ppdsdr_uartl & 0x40 )
      {
         eTPUInit();                 // Initialize eTPU pins for GPIO
         bIsModDev70 = TRUE;
      }
      else
      {
         bIsModDev70 = FALSE;
      }

      bInitModDev70 = TRUE;          // Confirm initialization of this condition
   }

   return bIsModDev70;
}


////////////////////////////////////////////////////////////////////////////////
// getdipsw
//
unsigned char getdipsw( void )
{
   if ( ModDev70Detect() )
   {
      //
      // Output a high/low signal depending on the state of the DIP switches.
      //
      ( J2[13] == 1 ) ? u8_dipsw |= 0x01 : u8_dipsw &= ~0x01;
      ( J2[12] == 1 ) ? u8_dipsw |= 0x02 : u8_dipsw &= ~0x02;
      ( J2[11] == 1 ) ? u8_dipsw |= 0x04 : u8_dipsw &= ~0x04;
      ( J2[9]  == 1 ) ? u8_dipsw |= 0x08 : u8_dipsw &= ~0x08;
      ( J2[10] == 1 ) ? u8_dipsw |= 0x10 : u8_dipsw &= ~0x10;
      ( J2[7]  == 1 ) ? u8_dipsw |= 0x20 : u8_dipsw &= ~0x20;
      ( J2[6]  == 1 ) ? u8_dipsw |= 0x40 : u8_dipsw &= ~0x40;
      ( J2[8]  == 1 ) ? u8_dipsw |= 0x80 : u8_dipsw &= ~0x80;

      return ( unsigned char ) u8_dipsw;
   }
   else
   {
      if ( !bInit )
      {
         InitChipBoardSelect();
      }

      return *pLed;
   }
}


////////////////////////////////////////////////////////////////////////////////
// putleds
//
void putleds( unsigned char c )
{
   if ( ModDev70Detect() )
   {
      //
      // Initialize standard UART1_RTS/CTS pins for GPIO and then output their
      // state based on input parameter.
      //
      sim.gpio.par_uart &= ~0x00F0;
      sim.gpio.podr_uartl = ~( ( c & 0x80 ) | ( ( c & 0x08 ) << 3 ) );
      sim.gpio.pddr_uartl = 0xC0;

      //
      // eTPU pins are initialized in ModDev70Detect() - only output their state
      // based on the input parameter.
      //
      ( c & 0x01 ) ? J2[15] = 0 : J2[15] = 1;
      ( c & 0x02 ) ? J2[16] = 0 : J2[16] = 1;
      ( c & 0x04 ) ? J2[18] = 0 : J2[18] = 1;
      ( c & 0x10 ) ? J2[17] = 0 : J2[17] = 1;
      ( c & 0x20 ) ? J2[19] = 0 : J2[19] = 1;
      ( c & 0x40 ) ? J2[20] = 0 : J2[20] = 1;
   }
   else
   {
      if ( !bInit )
      {
         InitChipBoardSelect();
      }

      *pLed = c;
   }
}


////////////////////////////////////////////////////////////////////////////////
// putdisp - This function is used only on the MOD-DEV-100 carrier board.
//
void putdisp( unsigned short w )
{
   BYTE b1 = ( w >> 8 );
   BYTE b2 = ( w & 0xFF );

   if ( !bInit )
   {
      InitChipBoardSelect();
   }

   pLed[1] = b1;
   pLed[2] = b2;
}
