/*
 * FormData.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 */

#include "formdata.h"
#include <stdlib.h>
#include <string.h>
#include <ucos.h>
#include <stdio.h>
#include <ctype.h>

#define NO_TIMEOUT 			0
#define MIN_RPM_LIMIT 		1
#define MAX_RPM_LIMIT		200
#define MAX_ROT_LIMIT		10000
#define MIN_ROT_LIMIT		1

extern BYTE display_error(const char * info, BYTE error);



/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData(OS_SEM * sem) {
	mySem = sem;
	minrpm_valid = true;
	maxrpm_valid = true;
	rot_valid = true;
	dir_valid = true;
}

/* Name: FormData Destructor
 * Description: Empty Destructor for the class
 * Inputs:	none
 * Outputs: none
 */
FormData::~FormData() {
	// TODO Auto-generated destructor stub

}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMaxRPM(char * rpm) {
	display_error("Error pending on semaphore in SetMaxRPM", OSSemPend(mySem, NO_TIMEOUT));
	int check_maxrpm = 0;
		if (checkNumericString(rpm)) {
			check_maxrpm = atoi(rpm);
		}
	if (check_maxrpm < MIN_RPM_LIMIT || check_maxrpm > MAX_RPM_LIMIT) {
		maxrpm_valid = false;
		display_error("Error posting to semaphore in SetMaxRPM", OSSemPost(mySem));
		return FORM_ERROR;
	}
	int_maxrpm = check_maxrpm;
	maxrpm_valid = true;
	display_error("Error posting to semaphore in SetMaxRPM", OSSemPost(mySem));
	return FORM_OK;
}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMaxRPM(void){

	return int_maxrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMinRPM(char * rpm) {
	display_error("Error pending on semaphore in SetMinRPM", OSSemPend(mySem, NO_TIMEOUT));
	int check_minrpm = 0;
	if (checkNumericString(rpm)) {
		check_minrpm = atoi(rpm);
	}
	if (check_minrpm < MIN_RPM_LIMIT || check_minrpm > int_maxrpm) {
		minrpm_valid = false;
		display_error("Error posting to semaphore in SetMinRPM", OSSemPost(mySem));
		return FORM_ERROR;
	}
	int_minrpm = check_minrpm;
	minrpm_valid = true;
	display_error("Error posting to semaphore in SetMinRPM", OSSemPost(mySem));
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetSteps(char * steps) {
	//	display_error("Error pending on semaphore in SetSteps", OSSemPend(&mySem, 0));
	//	int_steps = atoi(steps);
	//	display_error("Error posting to semaphore in SetSteps", OSSemPost(&mySem));
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int FormData::GetSteps (void) {
	return int_steps;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */

BYTE FormData::SetRotations(char * rot) {
	display_error("Error pending on semaphore in SetRotations", OSSemPend(mySem, 0));
	int check_rotations = 0;
	if (checkNumericString(rot)){
		check_rotations = atoi(rot);
	}
	if (check_rotations < MIN_ROT_LIMIT || check_rotations > MAX_ROT_LIMIT) {
		rot_valid = false;
		display_error("Error posting to semaphore in SetRotations", OSSemPost(mySem));
		return FORM_ERROR;
	}
	int_rotations = check_rotations;
	rot_valid = true;
	display_error("Error posting to semaphore in SetRotations", OSSemPost(mySem));
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetRotations(void){
	return int_rotations;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetDirection(char * dir){
	char* clockwise = "Clockwise";
	char* counterclockwise = "Counter-Clockwise";
	display_error("Error pending on semaphore in SetDirection", OSSemPend(mySem, 0));
	if (strncmp(dir, clockwise , strlen(clockwise)) == 0) { //arbitrary max string compare size
		direction = CW;
	} else if (strncmp(dir, counterclockwise, strlen(counterclockwise)) == 0 ) { //arbitrary max string compare size
		direction = CCW;
	} else {
		dir_valid = false;
		display_error("Error posting to semaphore in SetDirection", OSSemPost(mySem));
		return FORM_ERROR;
	}
	dir_valid = true;
	display_error("Error posting to semaphore in SetDirection", OSSemPost(mySem));
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetDirection(void){
	return direction;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetMode(void){
	return mode;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::Init(BYTE motor_mode){
	direction = UNSET;
	mode = motor_mode;
	int_maxrpm = 0;
	int_minrpm = 0;
	int_steps = 0;
	int_rotations = 0;
	return FORM_OK;
}

bool FormData::IsMinRPMValid(void) {
	return minrpm_valid;
}
bool FormData::IsMaxRPMValid(void){
	return maxrpm_valid;
}
bool FormData::IsRotationsValid(void){
	return rot_valid;;
}
bool FormData::IsDirectionValid(void){
	return dir_valid;
}

bool FormData::checkNumericString(char * str){
	char * pch;
	int i = 0;
	for (uint j =0; j < strlen(str); j++) {
		if(isalpha(str[j])) {
			iprintf("Found a letter\n");
			return false;
		}
	}
	pch = strtok(str, " .");
	char *first_tok = pch;
	while(pch != NULL){
		i++;
		if (i > 1){
			iprintf("more than one tok");
			return false;
		}
		pch = strtok(NULL, " .");
	}

	return true;
}
