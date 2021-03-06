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
#define KICK_BUTTON		1
#define CAPTURE_BUTTON	2
#define AIM_BUTTON		3
#define LEFT_BOOST_BUTTON		4
#define RIGHT_BOOST_BUTTON		5
//#define _BUTTON		6
#define TURN_BOOST_BUTTON		7
//#define _BUTTON		8
//#define _BUTTON		9

/* Operator (left) Stick Buttons
 *   - X		-
 *   - Y		- pickup
 *   - Z		- shooter speed
 *   - trigger	- shoot 
 */
//#define WINCH_BUTTON	1	// hard to hit by accident
//#define ARM_DOWN_BUTTON	2
//#define ARM_UP_BUTTON	3	// hard to hit by accident
#define ROLLER_STOP_BUTTON 3
#define OPERATOR_LEFT_SIDE_ROLLER_BUTTON	4
#define OPERATOR_RIGHT_SIDE_ROLLER_BUTTON	5
#define OPERATOR_PISTON_OUT_BUTTON		6
#define OPERATOR_PISTON_IN_BUTTON		7
//#define _BUTTON		8
//#define _BUTTON		9
#define OPERATOR_EM_OFF_BUTTON		10
#define OPERATOR_EM_ON_BUTTON		11

// Analog Inputs
#define GYRO_CHANNEL		1

// Digital Inputs
#define L_ENC_A_CHANNEL		1
#define L_ENC_B_CHANNEL		2
#define R_ENC_A_CHANNEL		3
#define R_ENC_B_CHANNEL		4
#define GATE_SENSOR			5
//#define R_IR_SENSOR			6	// not used--feel free to take it for any input you need
#define ULTRA_NEAR			7
#define ULTRA_FAR			8
#define PRESSURE_SWITCH_CHANNEL		9
// Driver Station Inputs--note that if more than one
// zone (d/mid/forward) switch is set we take the LOWEST
// and if none is set we become a midfielder
#define DEFENSE_SWITCH		10	//	0 = off		1 = Defensive player
#define MIDFIELD_SWITCH		11	//	0 = off		1 = Midfielder
#define FORWARD_SWITCH		12	//	0 = off		1 = Forward
#define PATTERN_1_SWITCH	13	// Binary code for 4 different patterns for each zone
#define PATTERN_2_SWITCH	14

// Relay outputs
#define COMPRESSOR_RELAY_CHANNEL 1 	// Compressor spike channel
#define ELECTROMAGNET_CHANNEL 2		// Electromagnet spike channel

// Solonoid Outputs
#define SOLENOID_SLOT			8 // solenoid controller slot
#define PISTON_ACTUATOR			3 // piston IN (back) channel
#define PISTON_RELEASE			4 // piston OUT (forward) channel

// CAN Devices
#define R_SIDE_ROLLER_JAG_ID	1	// side-pusher motor
#define L_WHEEL1_JAG_ID		3 
#define L_WHEEL2_JAG_ID		2 
#define R_WHEEL1_JAG_ID		4
#define R_WHEEL2_JAG_ID		5
#define ROLLER_JAG_ID		6
#define L_SIDE_ROLLER_JAG_ID	7	// side-pusher motor

#endif
