/* Nancy Minderman
* Dec 3, 2014
* nancy.minderman@ualberta.ca
* ECE315
*/

#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include "lcd.h"

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define TASK1_PRIO 	MAIN_PRIO + 1
#define TASK2_PRIO 	MAIN_PRIO + 2
#define TASK3_PRIO	MAIN_PRIO + 3
#define TASK4_PRIO	MAIN_PRIO + 4

/* Cursor Position Defines */
#define QUADRANT1 40
#define QUADRANT2 10
#define QUADRANT3 60
#define QUADRANT4 30

/* If you create new C subroutines you'll need to add them inside
 * the braces
 */

extern "C" {
void UserMain(void * pd);
void StartTask1(void);
void StartTask2(void);
void StartTask3(void);
void StartTask4(void);
void Task1Main(void * pd);
void Task2Main(void * pd);
void Task3Main(void * pd);
void Task4Main(void * pd);
}

extern BYTE display_error(const char * info, BYTE error);

/* Remember to add in a personal identifier next to the Lab 1 string
* I'll be looking for it in the demo
*/
const char * AppName= "CH and RT";
const char * Welcome="Welcome to ECE315-Winter2015";

/* These are the semaphores for exercise 4*/
static OS_SEM mySem;
static OS_SEM mySem2;
static OS_SEM mySem3;
static OS_SEM mySem4;

/* Task stacks for all the user tasks */
/* If you add a new task you'll need to add a new stack for that task */
DWORD Task1Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task2Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task3Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task4Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

/* This really needs to be in the heap so it's here and allocated
 * as a global class due to its size */
Lcd myLCD;

/* Everything in user land starts with this task */
void UserMain(void * pd) {

	BYTE err = OS_NO_ERR;

    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    EnableTaskMonitor();
    /* The LCD must be initialized before any task that may use the
     * class is created.
     */
    myLCD.Init(LCD_BOTH_SCR);
    myLCD.PrintString(LCD_UPPER_SCR, Welcome);
    OSTimeDly(TICKS_PER_SECOND*3);
	myLCD.Clear(LCD_BOTH_SCR);

    /* For exercise 3 and 4 put your semaphore and/or queue initializations
     * here.
     *
     *
     */
	// Initialize the semaphores we will use for ordered printing

	display_error("Error initializing mySem", OSSemInit (&mySem, 1));

	display_error("Error initializing mySem2", OSSemInit (&mySem2, 0));

	display_error("Error initializing mySem3", OSSemInit (&mySem3, 0));

	display_error("Error initializing mySem4", OSSemInit (&mySem4, 0));

	#ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started: %s\n", AppName);

    /* Start all of our user tasks here. If you add a new task
     * add it in here.
     */
    StartTask1();
    StartTask2();
    StartTask3();
    StartTask4();
}

/* Name: StartTask1
 * Description: Creates the functional part of the task, assigns the name
 * 				and priority.
 * Inputs: none
 * Outputs: none
 */
void StartTask1(void) {
	BYTE err = OS_NO_ERR;

	err = display_error ("Start Task 1: ",
					OSTaskCreatewName( Task1Main, 			//functional part of the task
					(void *)NULL,							// task data - not usually used
				 	(void *) &Task1Stk[USER_TASK_STK_SIZE], // task stack top
				 	(void *) &Task1Stk[0],					// task stack bottom
				 	TASK1_PRIO, "Task 1" )					// task priority and task name
	);
}

/* Name: Task1Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task1Main( void * pd) {
	/* place semaphore usage code inside the loop */
	while (1) {
			//Wait for sem1
			display_error("Error pending on mySem", OSSemPend(&mySem, 0));

			//Position Cursor
			myLCD.MoveCursor(LCD_LOWER_SCR, QUADRANT1);

			//Print 10 characters to the LCD screen
			for (int i = 0; i < 10; i++){
				myLCD.PrintChar(LCD_LOWER_SCR, 'a');
				OSTimeDly(TICKS_PER_SECOND*0.1);
			}

			//Post to the appropriate sem based on dip switch
			if (!getdipsw()){
				display_error("Error posting to mySem2", OSSemPost(&mySem2));
			} else {
				myLCD.Clear(LCD_BOTH_SCR);
				display_error("Error posting to mySem4", OSSemPost(&mySem4));
			}
	}


}
//Task 2
void StartTask2(void) {
	BYTE err = OS_NO_ERR;

	err = display_error ("Start Task 2: ",
					OSTaskCreatewName( Task2Main, 			//functional part of the task
					(void *)NULL,							// task data - not usually used
				 	(void *) &Task2Stk[USER_TASK_STK_SIZE], // task stack top
				 	(void *) &Task2Stk[0],					// task stack bottom
				 	TASK2_PRIO, "Task 2" )					// task priority and task name
	);
}

/* Name: Task2Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task2Main( void * pd) {
	/* place semaphore usage code inside the loop */
	while (1) {
			display_error("Error pending on mySem2", OSSemPend(&mySem2, 0));

			//Position the cursor
			myLCD.MoveCursor(LCD_LOWER_SCR, QUADRANT2);

			//Print 10 characters to the LCD screen
			for (int i = 0; i < 10; i++){
				myLCD.PrintChar(LCD_LOWER_SCR, 'b');
				OSTimeDly(TICKS_PER_SECOND*0.1);
			}

			//Post to the appropriate sem based on dip switch
			if (!getdipsw()){
				display_error("Error posting to mySem3", OSSemPost(&mySem3));
			} else {
				display_error("Error posting to mySem", OSSemPost(&mySem));
			}
	}


}

//Task 3
void StartTask3(void) {
	BYTE err = OS_NO_ERR;

	err = display_error ("Start Task 3: ",
					OSTaskCreatewName( Task3Main, 			//functional part of the task
					(void *)NULL,							// task data - not usually used
				 	(void *) &Task3Stk[USER_TASK_STK_SIZE], // task stack top
				 	(void *) &Task3Stk[0],					// task stack bottom
				 	TASK3_PRIO, "Task 3" )					// task priority and task name
	);
}

/* Name: Task3Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task3Main( void * pd) {
	/* place semaphore usage code inside the loop */
	while (1) {
		display_error("Error pending on mySem3", OSSemPend(&mySem3, 0));

			//Position the cursor
			myLCD.MoveCursor(LCD_UPPER_SCR, QUADRANT3);

			//Print 10 characters to the LCD screen
			for (int i = 0; i < 10; i++){
				myLCD.PrintChar(LCD_UPPER_SCR, 'c');
				OSTimeDly(TICKS_PER_SECOND*0.1);
			}

			//Post to the appropriate sem based on dip switch
			if (!getdipsw()){
				display_error("Error posting to mySem4", OSSemPost(&mySem4));
			} else {
				display_error("Error posting to nySem2", OSSemPost(&mySem2));
			}
	}


}

//Task 2
void StartTask4(void) {
	BYTE err = OS_NO_ERR;

	err = display_error ("Start Task 4: ",
					OSTaskCreatewName( Task4Main, 			//functional part of the task
					(void *)NULL,							// task data - not usually used
				 	(void *) &Task4Stk[USER_TASK_STK_SIZE], // task stack top
				 	(void *) &Task4Stk[0],					// task stack bottom
				 	TASK4_PRIO, "Task 4" )					// task priority and task name
	);
}

/* Name: Task4Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task4Main( void * pd) {
	/* place semaphore usage code inside the loop */
	while (1) {
		display_error("Error pending on mySem4", OSSemPend(&mySem4, 0));

			//Position the cursor
			myLCD.MoveCursor(LCD_UPPER_SCR, QUADRANT4);

			//Print 10 characters to the LCD scrren
			for (int i = 0; i < 10; i++){
				myLCD.PrintChar(LCD_UPPER_SCR, 'd');
				OSTimeDly(TICKS_PER_SECOND*0.1);
			}

			//Post to the appropriate sem based on dip switch
			if (!getdipsw()){
				myLCD.Clear(LCD_BOTH_SCR);
				display_error("Error posting to mySem", OSSemPost(&mySem));
			} else {
				display_error("Error posting to mySem3", OSSemPost(&mySem3));
			}
	}


}

