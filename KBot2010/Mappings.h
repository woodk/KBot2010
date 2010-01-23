#ifndef MAPPINGS_H
#define MAPPINGS_H

/*  USB 1 - The "right" joystick = Driver control 
 *  USB 2 - The "left" joystick  = Operator control
 */ 

/* Driver (right) Stick Buttons
 *   - X, Y		- Arcade Drive
 *   - Z 		- Torque setting programmed by button 7
 *   - trigger	- squared inputs for finer control
 */
#define HIGH_TORQUE_BUTTON	2
//#define WEAVE_BUTTON		3
#define PULSE_BUTTON		3
#define LEFT_90_BUTTON		4
#define RIGHT_90_BUTTON		5
#define TORQUE_BUTTON		6
#define SET_TORQUE_BUTTON	7
#define LEFT_180_BUTTON		8
#define RIGHT_180_BUTTON	9

/* Operator (left) Stick Buttons
 *   - X		-
 *   - Y		- pickup
 *   - Z		- shooter speed
 *   - trigger	- shoot 
 */
#define SHOOTER_SPEED_BUTTON	3
#define HOPPER_OPEN_BUTTON		6	// Not used
#define HOPPER_CLOSE_BUTTON		7	// Not used
#define RESET_CAMERA_BUTTON		9
#define REVERSE_SHOOTER_BUTTON	10
#define ADVANCE_SHOOTER_BUTTON	11
#define POWER_DUMP_BUTTON		6

// Analog Inputs
#define GYRO_CHANNEL		1
#define ACCEL_X_CHANNEL		2
#define ACCEL_Y_CHANNEL		3
#define ACCEL_Z_CHANNEL		4
#define L_CURRENT_CHANNEL	5
#define R_CURRENT_CHANNEL	6

// Digital Inputs
#define L_ENC_A_CHANNEL		1
#define L_ENC_B_CHANNEL		2
#define R_ENC_A_CHANNEL		3
#define R_ENC_B_CHANNEL		4
#define TEAM_SELECT			5
#define AUTO_LEFT_RIGHT		6
#define AUTO_MODE0			7
#define AUTO_MODE1			8

#define AUTO_STRAIGHT		0
#define AUTO_CURVE			2
#define AUTO_45_LOAD		1
#define AUTO_90_LOAD		3

#define AUTO_LEFT			0
#define AUTO_RIGHT			1

// Digital Outputs

// PWM Outputs
#define L_WHEEL_CHANNEL		1 
#define R_WHEEL_CHANNEL		2
#define SHOOTER_CHANNEL		4
#define PICKUP_CHANNEL		5
#define HOPPER_CHANNEL		6		// not used
#define L_SHIFT_CHANNEL		7
#define R_SHIFT_CHANNEL		8
#define CAMERA_VERTICAL		9

// Driver Station Inputs
//#define TEAM_SWITCH				8	//	0 = green/pink (blue)	1 = pink/green (Red)
//#define STRAIGHT_CURVE_SWITCH	7	//	0 = go straight		1 = curve
//#define CURVE_LEFT_RIGHT_SWITCH	6	//	0 = curve left		1 = curve right
//#define AUTOSHOOT_OVERRIDE		1

// Driver Station Analog 

// Driver Station Outputs
#define DS_LED_CAMERA_LOCK	1
#define DS_LED_IN_RANGE		2
#define DS_CAMERA_LOST		3
#define DS_START_STATE		4		// GoStraight, CurveLeft or CurveRight
#define DS_SPIN_STATE		5
#define DS_TRACK_STATE		6
#define DS_ATTACK_STATE		7
#define DS_END_STATE		8		// BackUp AND Circle

#endif