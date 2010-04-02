/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KICKER_H_
#define KICKER_H_

#include "Solenoid.h"
#include "Relay.h"
#include "DigitalModule.h"
class KBot;

#define PISTON_FORWARD_TIME	200		// 1.0 secs
#define RELOAD_TIME			200		// 1.0 sec
#define STOP_ROLLER_TIME	20		// 0.1 sec
#define KICK_TIME			75		// 0.375 secs

#define TEST_KICKER 1				// uncomment to activate testing

enum States {GET_CROSSBOW, TENSION_CROSSBOW, EM_ON, KICK, READY} ;

class HardwareFactory;

/**
 * High level hardware class for controlling all kicker functions
 */
class Kicker
{
public:
	Kicker(KBot *kbot, int kickerOutChannel, int kickerInChannel, int electromagnetChannel, HardwareFactory* pFactory);
	void	Init();
	void	Kick();
	void	onClock();

	void	onAction(States state);	
	bool Get(){return m_electromagnet->GetDigitalModule()->GetRelayForward();}
	
private:
	KBot		*m_kbot;
	States		state;
	int			counter;
	int			kickCounter;

	Solenoid	*m_kickerSolenoidOut;
	Solenoid	*m_kickerSolenoidIn;
	Relay		*m_electromagnet;
	
};
#endif
