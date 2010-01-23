/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef PICKUP_H
#define PICKUP_H

#include "Jaguar.h"

class SpeedController;

/**
 * Robot Pickup control
 */
class Pickup
{
public:
	explicit Pickup(UINT32 channel);
	Pickup(UINT32 slot, UINT32 channel);
	virtual ~Pickup();
	float Get();
	void Set(float value);
	void Drive(float speed, bool enabled);

private:
	void InitPickup();
	SpeedController *m_PickupMotor;
};
#endif

