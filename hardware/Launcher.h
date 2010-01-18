#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "Pickup.h"
#include "Shooter.h"
#include "Hopper.h"

enum LauncherStates { FIRE, STOP };

class Launcher
{
public:
	Launcher(UINT32 PickupChannel, UINT32 ShooterChannel, UINT32 HopperChannel);
	~Launcher();
	void Drive();
	void Fire();
	void Stop();

private: 
	LauncherStates LauncherState;
	int LauncherCycle;
	Pickup *m_pickup;
	Shooter *m_shooter;
	Hopper *m_hopper;
	
};
	
#endif
