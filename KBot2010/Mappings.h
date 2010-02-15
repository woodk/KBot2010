#ifndef MAPPINGS_H
#define MAPPINGS_H

#define KICK_STRENGTH_FORWARD	50
#define KICK_STRENGTH_MIDFIELD	100
#define KICK_STRENGTH_DEFENSE	200



/*  USB 1 - The "right" joystick = Driver control 
 *  USB 2 - The "left" joystick  = Operator control
 */ 

/* Driver (right) Stick Buttons
 *   - X, Y		- Arcade Drive
 *   - Z 		- Torque setting programmed by button 7
 *   - trigger	- squared inputs for finer control
 */
#define KICK_BUTTON		1
#define CAPTURE_BUTTON	2
#define AIM_BUTTON		3
//#define _BUTTON		4
//#define _BUTTON		5
//#define _BUTTON		6
//#define _BUTTON		7
//#define _BUTTON		8
//#define _BUTTON		9

/* Operator (left) Stick Buttons
 *   - X		-
 *   - Y		- pickup
 *   - Z		- shooter speed
 *   - trigger	- shoot 
 */
//#define _BUTTON		1
//#define _BUTTON		2
//#define _BUTTON		3
//#define _BUTTON		4
//#define _BUTTON		5
//#define _BUTTON		6
//#define _BUTTON		7
//#define _BUTTON		8
//#define _BUTTON		9

// Analog Inputs
#define GYRO_CHANNEL		1

// Digital Inputs
#define L_ENC_A_CHANNEL		1
#define L_ENC_B_CHANNEL		2
#define R_ENC_A_CHANNEL		3
#define R_ENC_B_CHANNEL		4
#define L_IR_SENSOR			5
#define R_IR_SENSOR			6
#define ULTRA_NEAR			7
#define ULTRA_FAR			8
#define PRESSURE_SWITCH_CHANNEL		9
// Driver Station Inputs--note that if more than one
// zone (d/mid/forward) switch is set we take the LOWEST
// and if none is set we become a midfielder
#define DEFENSE_SWITCH		10	//	0 = off		1 = Defensive player
#define MIDFIELD_SWITCH		11	//	0 = off		1 = Midfielder
#define FORWARD_SWITCH		12	//	0 = off		1 = Forward
#define PATTERN_A			13	// Binary code for 4 different patterns for each zone
#define PATTERN_B			14

// Relay outputs
#define COMPRESSOR_RELAY_CHANNEL 1 	// Compressor spike channel
#define ELECTROMAGNET_CHANNEL 2		// Electromagnet spike channel

// Solonoid Outputs
#define SOLENOID_SLOT			8 // solenoid controller slot
#define ARM_RELEASE				1 // arm release channel
#define PISTON_ACTUATOR			2 // piston actuator channel
#define PISTON_RELEASE			3 // piston release channel

// CAN Devices
#define L_WHEEL1_JAG_ID		3 
#define L_WHEEL2_JAG_ID		2 
#define R_WHEEL1_JAG_ID		4
#define R_WHEEL2_JAG_ID		5
#define WINCH_JAG_ID		1
#define ROLLER_JAG_ID		6

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
