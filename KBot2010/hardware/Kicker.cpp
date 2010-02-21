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
	m_electromagnet = new Relay(electromagnetChannel);
	counter=0;
	state=READY;
	reloadTime = 10;
	nFakeHoldCounter = 0;
	m_bOut = true;
}
void	Kicker::Init(int strength)
{
	counter=0;
	state=GET_CROSSBOW;
	reloadTime = strength;
	waitingToKick=false;
	nFakeHoldCounter = 0;
	m_bOut = true;
	m_electromagnet->Set(Relay::kForward);
}
void	Kicker::Kick()
{
	if (state==READY)
	{
		counter=0;
		state=KICK;
	}
	else
	{
		waitingToKick=true;
	}
}

void	Kicker::onClock()
{
	if (state==GET_CROSSBOW)
	{
		// turn on electromagnet
		// turn off kicker in
		// turn on kicker out
		m_electromagnet->Set(Relay::kOn);
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
		m_electromagnet->Set(Relay::kOn);
		m_kickerSolenoidIn->Set(true);
		m_kickerSolenoidOut->Set(false);
		
		// wait a certain time (depending on strength)
		counter++;
		if (counter>reloadTime)
		{
			state=READY;
			counter=0;
		}
	} else if (state==KICK)
	{
		//release EM
		m_electromagnet->Set(Relay::kOff);
		//wait a little
		counter++;
		state=GET_CROSSBOW;
		counter=0;
	} else //READY state
	{
		if (waitingToKick)
		{
			counter=0;
			waitingToKick=false;
			state=KICK;
		}
		else
		{
			// turn on kicker out & kicker in
			m_electromagnet->Set(Relay::kOn);
			++nFakeHoldCounter;
			if (2 == nFakeHoldCounter)
			{
				nFakeHoldCounter = 0;
				if (m_bOut)
				{
					m_kickerSolenoidIn->Set(true);
					m_kickerSolenoidOut->Set(false);
					nFakeHoldCounter = 0;
					m_bOut = false;
				}
				else
				{
					m_kickerSolenoidIn->Set(false);
					m_kickerSolenoidOut->Set(true);
					nFakeHoldCounter = 0;
					m_bOut = true;
				}
			}
			counter=0;
		}
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
		nFakeHoldCounter = 0;
	} else //READY state
	{
		m_electromagnet->Set(Relay::kOn);
		++nFakeHoldCounter;
		if (10 == nFakeHoldCounter)
		{
			nFakeHoldCounter = 0;
			if (m_bOut)
			{
				m_kickerSolenoidIn->Set(true);
				m_kickerSolenoidOut->Set(false);
				nFakeHoldCounter = 0;
				m_bOut = false;
			}
			else
			{
				m_kickerSolenoidIn->Set(false);
				m_kickerSolenoidOut->Set(true);
				nFakeHoldCounter = 0;
				m_bOut = true;
			}
		}
	}	
}
#endif

