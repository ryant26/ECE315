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
FormData::FormData() {
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

/* Name: SetMaxRPM
 * Description: setter for the max RPM of the stepper motor
 * Inputs: char * rpm is the RPM to set as the max RPM
 * Outputs: BYTE a form error or OK
 */
BYTE FormData::SetMaxRPM(char * rpm) {
	display_error("Error pending on semaphore in SetMaxRPM", OSSemPend(&mySem, NO_TIMEOUT));
	int check_maxrpm = 0;
		if (checkNumericString(rpm)) {
			check_maxrpm = atoi(rpm);
		}
	if (check_maxrpm < MIN_RPM_LIMIT || check_maxrpm > MAX_RPM_LIMIT) {
		maxrpm_valid = false;
		display_error("Error posting to semaphore in SetMaxRPM", OSSemPost(&mySem));
		return FORM_ERROR;
	}
	int_maxrpm = check_maxrpm;
	maxrpm_valid = true;
	display_error("Error posting to semaphore in SetMaxRPM", OSSemPost(&mySem));
	return FORM_OK;
}
/* Name: GetMaxRPM
 * Description:	Returns the max RPM for the stepper motor
 * Inputs:
 * Outputs: int the max RPM
 */
int  FormData::GetMaxRPM(void){
	display_error("Error pending on semaphore in GetMaxRPM", OSSemPend(&mySem, 0));
	int copy_maxrpm = int_maxrpm;
	display_error("Error posting to semaphore in GetMaxRPM", OSSemPost(&mySem));
	return copy_maxrpm;
}

/* Name: SetMinRPM
 * Description: setter for the min RPM of the stepper motor
 * Inputs: char * rpm is the RPM to set as the min RPM
 * Outputs: BYTE a form error or OK
 */
BYTE FormData::SetMinRPM(char * rpm) {
	display_error("Error pending on semaphore in SetMinRPM", OSSemPend(&mySem, NO_TIMEOUT));
	int check_minrpm = 0;
	if (checkNumericString(rpm)) {
		check_minrpm = atoi(rpm);
	}
	if (check_minrpm < MIN_RPM_LIMIT || check_minrpm > int_maxrpm) {
		minrpm_valid = false;
		display_error("Error posting to semaphore in SetMinRPM", OSSemPost(&mySem));
		return FORM_ERROR;
	}
	int_minrpm = check_minrpm;
	minrpm_valid = true;
	display_error("Error posting to semaphore in SetMinRPM", OSSemPost(&mySem));
	return FORM_OK;
}

/* Name: GetMinRPM
 * Description: Returns the min RPM for the stepper motor
 * Inputs:
 * Outputs: int the min RPM
 */
int  FormData::GetMinRPM(void) {
	display_error("Error pending on semaphore in GetMinRPM", OSSemPend(&mySem, 0));
	int copy_minrpm = int_minrpm;
	display_error("Error posting to semaphore in GetMinRPM", OSSemPost(&mySem));
	return copy_minrpm;
}

/* Name: SetSteps
 * Description: setter for the number of steps the motor should take
 * Inputs: char * steps the number of steps
 * Outputs: Byte form OK
 */
BYTE FormData::SetSteps(char * steps) {
	// Not used in lab 4. No Semaphore for constant
	return FORM_OK;
}

/* Name: GetSteps
 * Description: Returns the the number of steps for the stepper motor
 * Inputs:
 * Outputs: int the number of steps
 */
int FormData::GetSteps (void) {
	display_error("Error pending on semaphore in GetSteps", OSSemPend(&mySem, 0));
	int copy_steps = int_steps;
	display_error("Error posting to semaphore in GetSteps", OSSemPost(&mySem));
	return copy_steps;
}

/* Name: SetRotations
 * Description: setter for the number of rotations the motor should take
 * Inputs:	char * rot the number of rotations
 * Outputs: Byte Form OK or Form Error
 */

BYTE FormData::SetRotations(char * rot) {
	display_error("Error pending on semaphore in SetRotations", OSSemPend(&mySem, 0));
	int check_rotations = 0;
	if (checkNumericString(rot)){
		check_rotations = atoi(rot);
	}
	if (check_rotations < MIN_ROT_LIMIT || check_rotations > MAX_ROT_LIMIT) {
		rot_valid = false;
		display_error("Error posting to semaphore in SetRotations", OSSemPost(&mySem));
		return FORM_ERROR;
	}
	int_rotations = check_rotations;
	rot_valid = true;
	display_error("Error posting to semaphore in SetRotations", OSSemPost(&mySem));
	return FORM_OK;
}

/* Name: GetRotations
 * Description: gets the number of rotations for the stepper motor
 * Inputs:
 * Outputs: int number of rotations
 */
int  FormData::GetRotations(void){
	display_error("Error pending on semaphore in GetRotations", OSSemPend(&mySem, 0));
	int copy_rotations = int_rotations;
	display_error("Error posting to semaphore in GetRotations", OSSemPost(&mySem));
	return copy_rotations;
}

/* Name: SetDirection
 * Description: setter for the direction the motor should move
 * Inputs:
 * Outputs: BYTE Form OK or Form Error
 */
BYTE FormData::SetDirection(char * dir){
	char* clockwise = "Clockwise";
	char* counterclockwise = "Counter-Clockwise";
	display_error("Error pending on semaphore in SetDirection", OSSemPend(&mySem, 0));
	if (strncmp(dir, clockwise , strlen(clockwise)) == 0) { //arbitrary max string compare size
		direction = CW;
	} else if (strncmp(dir, counterclockwise, strlen(counterclockwise)) == 0 ) { //0 is string compare match
		direction = CCW;
	} else {
		dir_valid = false;
		display_error("Error posting to semaphore in SetDirection", OSSemPost(&mySem));
		return FORM_ERROR;
	}
	dir_valid = true;
	display_error("Error posting to semaphore in SetDirection", OSSemPost(&mySem));
	return FORM_OK;
}

/* Name: GetDirection
 * Description: gets the direction of movement for the stepper motor
 * Inputs:
 * Outputs: BYTE direction
 */
BYTE FormData::GetDirection(void){
	display_error("Error pending on semaphore in GetDirection", OSSemPend(&mySem, 0));
	BYTE copy_direction = direction;
	display_error("Error posting to semaphore in GetDirection", OSSemPost(&mySem));
	return copy_direction;
}

/* Name: GetMode
 * Description: getter for the mode of the stepper motor
 * Inputs:
 * Outputs: BYTE mode
 */
BYTE FormData::GetMode(void){
	display_error("Error pending on semaphore in GetMode", OSSemPend(&mySem, 0));
	BYTE copy_mode = mode;
	display_error("Error posting to semaphore in GetMode", OSSemPost(&mySem));
	return copy_mode;
}

/* Name: Init
 * Description: initilizer for this class, must be called prior to use
 * Inputs: BYTE motor_mode the mode for the stepper motor
 * Outputs: BYTE Form OK
 */
BYTE FormData::Init(BYTE motor_mode){
	direction = UNSET;
	mode = motor_mode;
	int_maxrpm = 0;
	int_minrpm = 0;
	int_steps = 0;
	int_rotations = 0;
	display_error("Error initializing semaphore", OSSemInit(&mySem, 1));
	return FORM_OK;
}

/* Name: IsMinRPMValid
 * Description: returns a boolean representing the valididty of last set MinRPMValue
 * Inputs: 
 * Outputs: Bool true if valid false otherwise
 */
bool FormData::IsMinRPMValid(void) {
	display_error("Error pending on semaphore in IsMinRPMValid", OSSemPend(&mySem, 0));
	bool copy_minrpm_valid = minrpm_valid;
	display_error("Error posting to semaphore in IsMinRPMValid", OSSemPost(&mySem));
	return copy_minrpm_valid;
}

/* Name: IsMaxRPMValid
 * Description: returns a boolean representing the valididty of last set maximum RPM
 * Inputs: 
 * Outputs: Bool true if valid false otherwise
 */
bool FormData::IsMaxRPMValid(void){
	display_error("Error pending on semaphore in IsMaxRPMValid", OSSemPend(&mySem, 0));
	bool copy_maxrpm_valid = maxrpm_valid;
	display_error("Error posting to semaphore in IsMinRPMValid", OSSemPost(&mySem));
	return copy_maxrpm_valid;
}

/* Name: IsRotationsValid
 * Description: returns a boolean representing the valididty of last set number of rotations
 * Inputs: 
 * Outputs: Bool true if valid false otherwise
 */
bool FormData::IsRotationsValid(void){
	display_error("Error pending on semaphore in IsRotationsValid", OSSemPend(&mySem, 0));
	bool copy_rot_valid = rot_valid;
	display_error("Error posting to semaphore in IsRotationsValid", OSSemPost(&mySem));
	return copy_rot_valid;

}

/* Name: IsDirectionValid
 * Description: returns a boolean representing the valididty of last set direction
 * Inputs: 
 * Outputs: Bool true if valid false otherwise
 */
bool FormData::IsDirectionValid(void){
	display_error("Error pending on semaphore in IsDirectionValid", OSSemPend(&mySem, 0));
	bool copy_dir_valid = dir_valid;
	display_error("Error posting to semaphore in IsDirectionValid", OSSemPost(&mySem));
	return copy_dir_valid;
}

/* Name: checkNumericString
 * Description: returns a boolean representing the valididty of an input string representing a numeric value
 * Inputs: 
 * Outputs: Bool true if valid false otherwise
 */
bool FormData::checkNumericString(char * str){
	char * pch;
	int i = 0;
	// Checking for letter characters in input
	for (uint j =0; j < strlen(str); j++) {
		if(isalpha(str[j])) {
			return false;
		}
	}
	// Checking for white space between inputs
	pch = strtok(str, " .");
	while(pch != NULL){
		i++;
		if (i > 1){
			return false;
		}
		pch = strtok(NULL, " .");
	}

	return true;
}
