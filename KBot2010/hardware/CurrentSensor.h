/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef CURRENTSENSOR_H_
#define CURRENTSENSOR_H_

#include "SensorBase.h"

class AnalogChannel;
class AnalogModule;

/**
 * Use a Current Sensor to return the current in Amps.
 */
class CurrentSensor : public SensorBase
{
public:
	static const UINT32 kOversampleBits = 10;
	static const UINT32 kAverageBits = 0;
	static const float kSamplesPerSecond = 50.0;
	static const float kDefaultVoltsPerAmp = 0.04;
	static const float kDefaultCentrePosition = 2.5;

	CurrentSensor(UINT32 slot, UINT32 channel);
	explicit CurrentSensor(UINT32 channel);
	explicit CurrentSensor(AnalogChannel *channel);
	explicit CurrentSensor(AnalogChannel &channel);
	virtual ~CurrentSensor();
	float GetCurrent();
	void SetSensitivity(float voltsPerAmp);
	void SetCentrePosition(float centrePosition);

private:
	void InitCurrentSensor();

	AnalogChannel *m_analog;
	float m_voltsPerAmp;
	float m_centrePosition;
	bool m_channelAllocated;
};
#endif
