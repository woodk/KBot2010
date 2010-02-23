/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KICKER_H_
#define KICKER_H_

#include "Solenoid.h"
#include "Relay.h"

#define PISTON_FORWARD_TIME	25		// 0.5 secs
#define RELOAD_TIME			25		// 0.5 sec
#define KICK_TIME			5		// 0.1 secs

#define TEST_KICKER 1				// uncomment to activate testing

enum States {GET_CROSSBOW, TENSION_CROSSBOW, EM_ON, KICK, READY} ;

/**
 * High level hardware class for controlling all kicker functions
 */
class Kicker
{
public:
	Kicker(int kickerOutChannel, int kickerInChannel, int electromagnetChannel);
	void	Init();
	void	Kick();
	void	onClock();

	void	onAction(States state);	
	
private:
	States		state;
	int			counter;
	int			kickCounter;

	Solenoid	*m_kickerSolenoidOut;
	Solenoid	*m_kickerSolenoidIn;
	Relay		*m_electromagnet;
	
};
#endif
