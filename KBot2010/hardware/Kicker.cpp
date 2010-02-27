/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "HardwareFactory.h"
#include "Kicker.h"
#include "Mappings.h"
#include "KBot.h"

/**
 * Kicker constructor--set up lower-level hardware
 * 
 */
Kicker::Kicker(KBot *kbot, int kickerOutChannel, int kickerInChannel, int electromagnetChannel, HardwareFactory *pFactory)
{
	m_kbot = kbot;
	m_kickerSolenoidOut = pFactory->BuildSolenoid(kickerOutChannel);
	m_kickerSolenoidIn = pFactory->BuildSolenoid(kickerInChannel);
	m_electromagnet = pFactory->BuildRelay(electromagnetChannel,Relay::kForwardOnly);
	counter=0;
	state=GET_CROSSBOW;
	kickCounter = 0;
}
void	Kicker::Init()
{
	counter=0;
	state=GET_CROSSBOW;
	printf(">>>>>> state=GET_CROSSBOW\n");
	m_electromagnet->Set(Relay::kForward);
}
void	Kicker::Kick()
{
	state=KICK;
}

void	Kicker::onClock()
{
	counter++;
	if (state==GET_CROSSBOW)
	{
		// turn on electromagnet
		// turn off kicker in
		// turn on kicker out
		m_electromagnet->Set(Relay::kForward);
		m_kickerSolenoidIn->Set(false);
		m_kickerSolenoidOut->Set(true);
		// wait for certain time (to move piston forward)
		if (counter>PISTON_FORWARD_TIME)
		{
			state=TENSION_CROSSBOW;
			counter=0;
			printf(">>>>>> Switch to TENSION_CROSSBOW\n");
		}
	} else if (state==TENSION_CROSSBOW)
	{
		// turn on kicker in
		// turn off kicker out
		m_electromagnet->Set(Relay::kForward);
		m_kickerSolenoidIn->Set(true);
		m_kickerSolenoidOut->Set(false);
		if (counter>RELOAD_TIME)
		{
			counter=0;
			kickCounter = 0;
			printf(">>>>>> Reloaded; ready to shoot\n");
		}		
	} else if (state==KICK)
	{
		/*if (kickCounter < STOP_ROLLER_TIME)
		{
			m_kbot->getRollerMotor()->Set(0.0);
		}
		else*/ if (kickCounter < /*STOP_ROLLER_TIME +*/ KICK_TIME)
		{
			printf("Kicker: KICK\n");
			//release EM
			m_electromagnet->Set(Relay::kOff);
			counter = 0;
			++kickCounter;
		}
		else
		{
			printf(">>>>>> Kicked -- reloading\n");
			state=GET_CROSSBOW;
		}
	}
}

void	Kicker::onAction(States state)
{
	if (state==GET_CROSSBOW)
	{
		state=GET_CROSSBOW;
		// turn on electromagnet
		// turn off kicker in
		// turn on kicker out
		m_kickerSolenoidIn->Set(false);
		m_kickerSolenoidOut->Set(true);
	} else if (state==TENSION_CROSSBOW)
	{
		state=TENSION_CROSSBOW;
		// turn on kicker in
		// turn off kicker out
		m_kickerSolenoidIn->Set(true);
		m_kickerSolenoidOut->Set(false);
	} else if (state==EM_ON)
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


