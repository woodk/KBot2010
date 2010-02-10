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
#define IR_SENSOR_1			5
// Driver Station Inputs--note that if more than one
// zone (d/mid/forward) switch is set we take the LOWEST
// and if none is set we become a midfielder
#define DEFENSE_SWITCH		6	//	0 = off		1 = Defensive player
#define MIDFIELD_SWITCH		7	//	0 = off		1 = Midfielder
#define FORWARD_SWITCH		8	//	0 = off		1 = Forward
#define ULTRA_NEAR			9
#define ULTRA_FAR			10

#define AUTO_STRAIGHT		0
#define AUTO_CURVE			2
#define AUTO_45_LOAD		1
#define AUTO_90_LOAD		3

#define AUTO_LEFT			0
#define AUTO_RIGHT			1

// Digital Outputs

#define SOLENOID_SLOT			8 // solenoid controller slot
#define ARM_RELEASE				1 // arm release channel
#define PISTON_ACUTATOR			2 // piston accuator channel
#define PISTON_RELEASE			3 // piston release channel

// PWM Outputs
#define L_WHEEL_CHANNEL		1 
#define R_WHEEL_CHANNEL		2
#define SHOOTER_CHANNEL		4
#define PICKUP_CHANNEL		5
#define HOPPER_CHANNEL		6		// not used
#define L_SHIFT_CHANNEL		7
#define R_SHIFT_CHANNEL		8
#define CAMERA_VERTICAL		9

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
