/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef HOPPER_H
#define HOPPER_H

//#include <stdlib.h>
//#include <VxWorks.h>

#include "Jaguar.h"

class SpeedController;

/**
 * Robot hopper control
 */
class Hopper
{
public:
	explicit Hopper(UINT32 channel);
	Hopper(UINT32 slot, UINT32 channel);
	virtual ~Hopper();
	float Get();
	void Set(float value);
	void Drive(float speed, bool enabled);

private:
	void InitHopper();
	SpeedController *m_HopperMotor;
};
#endif

