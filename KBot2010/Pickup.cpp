/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/


#include "Pickup.h"
//#include "DigitalModule.h"

/**
 * Constructor that assumes the default digital module.
 * 
 * @param channel The PWM channel on the digital module that the Jaguar is attached to.
 */
Pickup::Pickup(UINT32 channel) 
{
	m_PickupMotor = new Jaguar(channel);
	m_PickupMotor->Set(0);
}

/**
 * Constructor that specifies the digital module.
 * 
 * @param slot The slot in the chassis that the digital module is plugged into.
 * @param channel The PWM channel on the digital module that the Jaguar is attached to.
 */
Pickup::Pickup(UINT32 slot, UINT32 channel)
{
	m_PickupMotor = new Jaguar(slot, channel);
	m_PickupMotor->Set(0);
}

Pickup::~Pickup()
{
}

/**
 * Set the PWM value.  
 * 
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 * 
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void Pickup::Set(float speed)
{
	m_PickupMotor->Set(speed);
}

/**
 * Set the PWM value.  
 * 
 * @param speed The speed value between -1.0 and 1.0 to set.
 * @param enabled Whether or not to use the speed value.
 */
void Pickup::Drive(float speed, bool enabled)
{
	if (enabled) {
		m_PickupMotor->Set(speed);
	} else {
		m_PickupMotor->Set(0);
	}
}

/**
 * Get the recently set value of the PWM.
 * 
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Pickup::Get()
{
	return m_PickupMotor->Get();
}
