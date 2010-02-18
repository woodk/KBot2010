/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KICKER_H_
#define KICKER_H_

#include "Solenoid.h"
#include "Relay.h"

#define PISTON_FORWARD_TIME	100		// 2 secs
#define KICK_TIME			100		// 2 secs

enum States {GET_CROSSBOW, TENSION_CROSSBOW, KICK, READY} ;

/**
 * High level hardware class for controlling all kicker functions
 */
class Kicker
{
public:
	Kicker(int kickerOutChannel, int kickerInChannel, int electromagnetChannel);
	void	Init(int strength);
	void	Kick();
	void	onClock();
	void	SetState(States newState) {state = newState;} // for H/W debug
	
	bool 	getIsReady() {return READY == state;}

private:
	States		state;
	int			counter;
	int			reloadTime;
	bool		waitingToKick;
	Solenoid	*m_kickerSolenoidOut;
	Solenoid	*m_kickerSolenoidIn;
	Relay		*m_electromagnet;

};
#endif
