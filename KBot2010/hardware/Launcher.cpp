#include "Launcher.h"

Launcher::Launcher(UINT32 PickupChannel, UINT32 ShooterChannel, UINT32 HopperChannel)
{
	m_pickup = new Pickup(PickupChannel);
	m_shooter = new Shooter(ShooterChannel);
	m_hopper = new Hopper(HopperChannel);
	LauncherState=STOP;
}

Launcher::~Launcher()
{
	;
}

void Launcher::Drive()
{
	if (LauncherState == FIRE)
	{
		LauncherCycle+=1;
		if (LauncherCycle <= 100)
		{
			m_pickup->Drive(-0.5, true);
		}
		else if (LauncherCycle > 100)
		{
			m_pickup->Drive(1.0, true);
			m_shooter->Drive(1.0, true);
		}
	}
	else if (LauncherState == STOP)
	{
		m_pickup->Drive(0.0, true);
		m_shooter->Drive(0.0, true);
	}
}

void Launcher::Fire()
{
	LauncherCycle=0;
	LauncherState=FIRE;
}	

void Launcher::Stop()
{
	LauncherState=STOP;
}
