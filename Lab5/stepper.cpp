#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>

#include <ucos.h>
#include <cfinter.h>

#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>

#include <pins.h>
#include <math.h>

#include <sim5234.h>
#include "stepper.h"
#include "motorconstants.h"


/* Name: Stepper
 * Description:	Class constructor
 * Inputs: master channel for complete configuration. Must be between 1 and 12
 * for a bipolar stepper motor.
 * Outputs: None
 */
Stepper::Stepper(int mstr_channel, int accel_size)
{
	table_size = accel_size;
	master_channel = mstr_channel;
	slave_channel = master_channel + SLAVE_CHANNEL_OFFSET;
};

/* Name:Init
 * Description: Chooses a configuration to run the motor with. Supports
 * Full step or half step (exercise for the students)
 * Inputs: int mode must be one of ECE315_ETPU_SM_FULL_STEP_MODE or
 * ECE315_ETPU_SM_HALF_STEP_MODE
 * 			unsigned long start is start period. Range limit from SM_MIN_PERIOD = 1
 * 			to SM_MAX_PERIOD = 0x007fffff
 * 			unsigned long slew is slew period. Range limit from SM_MIN_PERIOD = 1
 * 			to SM_MAX_PERIOD = 0x007fffff
 * Outputs: None
 */
void Stepper::Init(int mode, unsigned long start, unsigned long slew)
{
	//Ensure Start and Slew Periods are in the correct range
	PutInRange(slew,  SM_MIN_PERIOD, SM_MAX_PERIOD);
	PutInRange(start, SM_MIN_PERIOD, SM_MAX_PERIOD);

	my_output_mode = mode;

	my_accel_tbl = new unsigned short [table_size];
	build_table(start, slew);

	/************** Full-Step Mode ***************/
	if (my_output_mode == ECE315_ETPU_SM_FULL_STEP_MODE)
	{
		//Define steps per revolution for FULL STEP
		steps_per_rev = STEPS_PER_REV_FULL_STEP;

		//Initialize the two drive signals
		if (fs_etpu_sm_init(
				master_channel,				// Stepper Stepper master channel
				ECE315_ETPU_SM_FULL_STEP, // Use two phase full step configuration
				0,						// Motor Start Position
				start,					// Start Period
				slew,					// Slew Period
				my_accel_tbl, 			// Pointer to acceleration table
				table_size				// Size of Acceleration table
		) == FS_ETPU_ERROR_MALLOC) iprintf ("MALLOC FAIL FULL!!!");

		/* Pull enable lines high permanently */
		/* J2 pins for the two enables are 19 and 20
		 * consult the lab 5 schematic to verify*/
		J2[J2_SM_ENABLE_A] = HIGH;
		J2[J2_SM_ENABLE_B] = HIGH;


		//Enable the Drive Lines
		fs_etpu_sm_enable(master_channel, FS_ETPU_PRIORITY_LOW);
	}
	/************** Half-Step Mode ***************/
	else {
		//Define steps per revolution for HALF STEP
		steps_per_rev = STEPS_PER_REV_HALF_STEP;

		//Initialize enable signal
		if (fs_etpu_sm_init(
				slave_channel,
				ECE315_ETPU_SM_2PHASE_HALF_STEP_ENA,
				0,				// Motor Start Position
				start,
				slew,
				my_accel_tbl,
				table_size) == FS_ETPU_ERROR_MALLOC) iprintf ("MALLOC FAIL FULL!!!");

		fs_etpu_sm_enable(slave_channel, FS_ETPU_PRIORITY_LOW);

		// Initialize drive signal
		if (fs_etpu_sm_init(
				master_channel,
				ECE315_ETPU_SM_2PHASE_HALF_STEP_DRIVE,
				0,				// Motor Start Position
				start,
				slew,
				my_accel_tbl,
				table_size) == FS_ETPU_ERROR_MALLOC) iprintf ("MALLOC FAIL FULL!!!");

		fs_etpu_sm_enable(master_channel, FS_ETPU_PRIORITY_LOW);
	}
	delete 	my_accel_tbl;
}

//
/* Name:NewAccelTable
 * Description: Remake the acceleration table, without having to use more pram.
 * Uses current values for start and slew periods.
 * Inputs: None
 * Outputs: None
 */
void Stepper::NewAccelTable()
{
	my_accel_tbl = new unsigned short [table_size];
	build_table(GetStartPeriod(), GetSlewPeriod());

	fs_etpu_sm_table(master_channel, my_accel_tbl);
	if (my_output_mode == ECE315_ETPU_SM_HALF_STEP_MODE) {
		fs_etpu_sm_table(slave_channel, my_accel_tbl);
	}

	delete 	my_accel_tbl;
}

/* Name: Step
 * Description: Moves the motor the specified number of steps.
 * Inputs: int steps Positive number should rotate CW and negative
 * 	number should rotate CCW
 * Outputs: None.
 */
void Stepper::Step(int steps)
{
	// movement is always relative to the current position.
	fs_etpu_sm_set_dp(master_channel, fs_etpu_sm_get_cp(master_channel) + steps);
	if (my_output_mode == ECE315_ETPU_SM_HALF_STEP_MODE) {
		fs_etpu_sm_set_dp(slave_channel, fs_etpu_sm_get_cp(slave_channel) + steps);
	}
}

/* Name: SetSlewPeriod
 * Description: Change the slew period. Max RPM will result from being
 * in max slew.
 * Inputs: int slew. Valid eTPU numbers are 1-> 0x007fffff
 * Outputs: unsigned int new slew value
 */
unsigned int Stepper::SetSlewPeriod(unsigned int slew)
{

	PutInRange(slew, SM_MIN_PERIOD, SM_MAX_PERIOD);

	fs_etpu_sm_set_sp(master_channel, slew);

	if (my_output_mode == ECE315_ETPU_SM_HALF_STEP_MODE) {
		fs_etpu_sm_set_sp(slave_channel, slew);
	}
	NewAccelTable();

	return slew;

}
/* Name: SetStartPeriod
 * Description: Change the start period. Min RPM is the human
 * version of start period.
 * Inputs: * Inputs: int start. Valid eTPU numbers are 1-> 0x007fffff
 * Outputs: unsigned int new start value
 */
unsigned int Stepper::SetStartPeriod(unsigned int start)
{
	PutInRange(start, SM_MIN_PERIOD, SM_MAX_PERIOD);

	fs_etpu_sm_set_st(master_channel, start);

	if (my_output_mode == ECE315_ETPU_SM_HALF_STEP_MODE) {
		fs_etpu_sm_set_st(slave_channel, start);
	}
	NewAccelTable();

	return start;
}

/* Create a new acceleration table.
 * The period is calculated to be the great of:
 * 		Start_Period * my_accel_tbl[i] / 0xFFFF
 * 		and
 * 		Slew_Period
 *
 * This function will calculate so the the frequency linearly increased
 * from Tstart to Tslew, to create a constant acceleration.
 * my_accel_tbl[i] = (0xFFFF * Tslew)
 * 					-------------------------
 *			(Tslew + (Tstart - Tslew)* i / table_size)
 */
/* Name: build_table
 * Description: Calculates the values for the new acceleartion table.
 * This needs to recalculated everytime the start or slew period is changed.
 * Inputs: unsigned long start and slew
 * Outputs: None
 */
void Stepper::build_table(unsigned long start, unsigned long slew)
{
	double val;

	for (int i = 0; i < table_size; i++){
		val = D_Div( D_Mul(0xFFFF,slew) ,slew+ D_Mul( (start-slew),D_Div(i+1,table_size) ) );
		my_accel_tbl[i] = (unsigned short)(val);
	}
}
//get current position
/* Name: GetCurrentPosition
 * Description: Returns the eTPU-stored current position of master channel configuration
 * Inputs: None
 * Outputs: This should be an unsigned 24 bit number
 */
unsigned int Stepper::GetCurrentPosition(void)
{
	return fs_etpu_sm_get_cp(master_channel);
}

/* Name:GetDesiredPosition
 * Description: Returns the eTPU-stored desired position of master channel configuration
 * Inputs: None
 * Outputs: This should be an unsigned 24 bit number
 */
unsigned int Stepper::GetDesiredPosition(void)
{
	return fs_etpu_sm_get_dp(master_channel);
}

/* Name:GetSlewPeriod
 * Description: Returns the eTPU-stored slew period of master channel configuration
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::GetSlewPeriod(void)
{
	return fs_etpu_sm_get_sp(master_channel);
}


/* Name:GetStartPeriod
 * Description:Returns the eTPU-stored start period of master channel configuration
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::GetStartPeriod(void)
{
	return fs_etpu_sm_get_st(master_channel);
}

/* Name:ConvertRPMToPeriodTicks
 * Description: Converts RPM to period ticks/step that the eTPU likes
 * Uses the freq of the tcr1 clock to calculate the number
 * of period ticks per step
 * Inputs: unsigned int rpm
 * Outputs: unsigned int
 */
unsigned int Stepper::ConvertRPMToPeriodTicks(unsigned int rpm) {
	unsigned int ticks = 0;
	ticks = SECONDS_PER_MINUTE * ( etpu_a_tcr1_freq / ( rpm * steps_per_rev));
	return ticks;
}

/* Name:SetStartPeriodUsingRPM
 * Description:Sets the start period using the more human friendly RPM
 * Inputs: unsigned int rpm
 * Outputs: unsigned int ticks/step
 */
unsigned int Stepper::SetStartPeriodUsingRPM(unsigned int rpm) {
	unsigned int ticks = 0;
	ticks = ConvertRPMToPeriodTicks(rpm);
	SetStartPeriod(ticks);
	return ticks;
}

/* Name:SetSlewPeriodUsingRPM
 * Description:Sets the slew period using the more human friendly RPM
 * Inputs: unsigned int rpm
 * Outputs: unsigned int ticks/step
 */
unsigned int Stepper::SetSlewPeriodUsingRPM(unsigned int rpm){
	unsigned int ticks = 0;
	ticks = ConvertRPMToPeriodTicks(rpm);
	SetSlewPeriod(ticks);
	return ticks;
}

/* Name:Stop
 * Description: Stops the motor by immediately disabling and re-enabling the stepper
 * motor configuration associated with the master channel
 * Inputs:
 * Outputs:
 */
void Stepper::Stop(void ) {
	fs_etpu_sm_disable( master_channel, FS_ETPU_SM_DISABLE_LOW);
	fs_etpu_sm_enable( master_channel, FS_ETPU_PRIORITY_LOW);

	if (my_output_mode == ECE315_ETPU_SM_HALF_STEP_MODE) {
		fs_etpu_sm_disable( slave_channel, FS_ETPU_SM_DISABLE_LOW);
		fs_etpu_sm_enable( slave_channel, FS_ETPU_PRIORITY_LOW);
	}
}

