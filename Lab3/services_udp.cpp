#include <stdio.h>
#include <stdlib.h>
#include <startnet.h>
#include <udp.h>
#include <string.h>
#include "services_udp.h"

// Task Stacks for our UDP tasks
asm( " .align 4 " );
DWORD UdpEchoStack[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD UdpChargenStack[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

extern BYTE display_error(const char * info, BYTE error);

/* Name: StartEchoServer
 * Description: Creates the task that implements the echo server.
 * Actual task code is in UdpEchoTask
 * Inputs: none
 * Outputs: none
 */
void StartEchoServer(void) {

	BYTE err = OS_NO_ERR;


	err = display_error("Creating Echo Server", OSTaskCreatewName( UdpEchoTask,
					(void *) ECHO_PORT,
					&UdpEchoStack[USER_TASK_STK_SIZE],
					UdpEchoStack,
					UDP_ECHO_PRIO, "Echo Task"));

}

/* Name: UdpEchoTask
 * Description: Implements an echo server that returns all text sent to it.
 * Inputs: void * pd contains the port to listen on
 * Outputs: none
 */
void UdpEchoTask( void *pd )
{
   int port = ( int ) pd;

   // Create a UDP socket for receiving
   int UdpFd = CreateRxUdpSocket( port );
   if ( UdpFd <= 0 )
   {
      iprintf("Error Creating UDP Listen Socket: %d\r\n", UdpFd);
      while (1)
         OSTimeDly(TICKS_PER_SECOND);
   }
   else
   {
      iprintf( "Listening for UDP packets on port %d\r\n", port );
   }

   while (1)
   {
      IPADDR SrcIpAddr;  // UDP packet source IP address
      WORD   LocalPort;  // Port number UDP packet was sent to
      WORD   SrcPort;    // UDP packet source port number
      char   buffer[MAX_UDP_BUF_LEN+1];

      int len = recvfrom( UdpFd, (BYTE *)buffer, MAX_UDP_BUF_LEN, &SrcIpAddr, &LocalPort, &SrcPort );
      buffer[len] = '\0';

      iprintf( "\r\nReceived a UDP packet with %d bytes from :", len );
      ShowIP( SrcIpAddr );
      iprintf( "\r\n%s\r\n", buffer );

      // Now send it right back
      {
      	UDPPacket pkt;
      	pkt.SetSourcePort(LocalPort);
      	pkt.SetDestinationPort(SrcPort);
      	pkt.AddData( (BYTE *)buffer, (WORD) strlen(buffer));
      	pkt.AddDataByte(0);
      	pkt.Send( SrcIpAddr);
      }
   }
}

/* Name: StartChargenServer
 * Description: Creates the task that implements the chargen server
 * Actual task code is in UdpChargenTask
 * Inputs: none
 * Outputs: none
 */
void StartChargenServer(void) {

	BYTE err = OS_NO_ERR;


	err = display_error ("Creating Chargen Server", OSTaskCreatewName(  UdpChargenTask,
					(void *) CHARGEN_PORT,
					&UdpChargenStack[USER_TASK_STK_SIZE],
					UdpChargenStack,
					UDP_CHARGEN_PRIO, "Chargen Task"));

}

/* Name: UdpChargenTask
 * Description: Implements a chargen server that returns a fixed string.
 * Inputs: void *pd contains the port to listen on.
 * Outputs: none
 */
void UdpChargenTask( void *pd )
{
   int port = ( int ) pd;
   const char   chargen_str[] = { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz!@#$%^&*-="};
   char 	buffer[MAX_UDP_BUF_LEN+1];

   // Create a UDP socket for receiving
   int UdpFd = CreateRxUdpSocket( port );
   if ( UdpFd <= 0 )
   {
      iprintf("Error Creating UDP Listen Socket: %d\r\n", UdpFd);
      while (1)
         OSTimeDly(TICKS_PER_SECOND);
   }
   else
   {
      iprintf( "Listening for UDP packets on port %d\r\n", port );
   }

   while (1)
   {
      IPADDR SrcIpAddr;  // UDP packet source IP address
      WORD   LocalPort;  // Port number UDP packet was sent to
      WORD   SrcPort;    // UDP packet source port number


      int len = recvfrom( UdpFd, (BYTE *)buffer, MAX_UDP_BUF_LEN, &SrcIpAddr, &LocalPort, &SrcPort );
      buffer[len] = '\0';

      {
      	UDPPacket pkt;
      	pkt.SetSourcePort(LocalPort);
      	pkt.SetDestinationPort(SrcPort);
      	pkt.AddData((BYTE *) chargen_str, (WORD) strlen(chargen_str));
      	pkt.AddDataByte(0);
      	pkt.Send( SrcIpAddr);
      }
   }
}
