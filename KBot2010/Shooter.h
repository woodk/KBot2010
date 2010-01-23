/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef SHOOTER_H
#define SHOOTER_H

#include "Victor.h"

class SpeedController;


/**
 * Robot shooter control
 */
class Shooter 
{
public:
	explicit Shooter(UINT32 channel);
	Shooter(UINT32 slot, UINT32 channel);
	virtual ~Shooter();
	float Get();
	void Set(float value);
	void Drive(float speed, bool enabled);

private:
	void InitShooter();
	SpeedController *m_ShooterMotor;
};
#endif

