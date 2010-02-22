/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Kicker.h"
#include "Mappings.h"
#include "KBot.h"

/**
 * Kicker constructor--set up lower-level hardware
 * 
 */
Kicker::Kicker(int kickerOutChannel, int kickerInChannel, int electromagnetChannel)
{
	m_kickerSolenoidOut = new Solenoid(kickerOutChannel);
	m_kickerSolenoidIn = new Solenoid(kickerInChannel);
	m_electromagnet = new Relay(electromagnetChannel,Relay::kForwardOnly);
	counter=0;
	state=GET_CROSSBOW;
	reloadTime = 10;
}
void	Kicker::Init(int strength)
{
	counter=0;
	state=GET_CROSSBOW;
	reloadTime = strength;
	m_electromagnet->Set(Relay::kForward);
}
void	Kicker::Kick()
{
	state=KICK;
}

void	Kicker::onClock()
{
	if (state==GET_CROSSBOW)
	{
		// turn on electromagnet
		// turn off kicker in
		// turn on kicker out
		m_electromagnet->Set(Relay::kForward);
		m_kickerSolenoidIn->Set(false);
		m_kickerSolenoidOut->Set(true);
		// wait for certain time (to move piston forward)
		counter++;
		if (counter>PISTON_FORWARD_TIME)
		{
			state=TENSION_CROSSBOW;
			counter=0;
		}
	} else if (state==TENSION_CROSSBOW)
	{
		// turn on kicker in
		// turn off kicker out
		m_electromagnet->Set(Relay::kForward);
		m_kickerSolenoidIn->Set(true);
		m_kickerSolenoidOut->Set(false);
		
	} else if (state==KICK)
	{
		printf("Kicker: KICK\n");
		//release EM
		m_electromagnet->Set(Relay::kOff);
		state=GET_CROSSBOW;
		counter = 0;
	}
}


#ifdef TEST_KICKER
void	Kicker::onTest(States state)
{
	if (state==GET_CROSSBOW)
	{
		// turn on electromagnet
		// turn off kicker in
		// turn on kicker out
		m_kickerSolenoidIn->Set(false);
		m_kickerSolenoidOut->Set(true);
	} else if (state==TENSION_CROSSBOW)
	{
		// turn on kicker in
		// turn off kicker out
		m_kickerSolenoidIn->Set(true);
		m_kickerSolenoidOut->Set(false);
	} else if (state==TEST_EM)
	{
		m_electromagnet->Set(Relay::kForward);		
	} else if (state==KICK)
	{
		//release EM
		m_electromagnet->Set(Relay::kOff);
		state = GET_CROSSBOW;	
		counter = 0;
	}	
}
#endif

