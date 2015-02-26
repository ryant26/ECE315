/*
 * services_udp.h
 *
 *  Created on: Oct 18, 2012
 *      Author: Nancy Minderman
 *      		nancy.minderman@ualberta.ca
 *
 *      Modified for the fall 2012 term.
 */

#ifndef SERVICES_UDP_H_
#define SERVICES_UDP_H_

#define UDP_ECHO_PRIO 			MAIN_PRIO + 2
#define UDP_CHARGEN_PRIO		MAIN_PRIO + 3
#define ECHO_PORT		6
#define CHARGEN_PORT	17
#define	MAX_UDP_BUF_LEN 20

extern "C"
{
	void UdpEchoTask( void *pd );
	void StartEchoServer(void);
	void UdpChargenTask(void *pd);
	void StartChargenServer(void);
}

#endif /* SERVICES_UDP_H_ */
