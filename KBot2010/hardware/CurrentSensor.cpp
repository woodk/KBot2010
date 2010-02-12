/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CurrentSensor.h"
#include "AnalogChannel.h"
#include "AnalogModule.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIStatus.h"


/**
 * Initialize the Current Sensor.
 */
void CurrentSensor::InitCurrentSensor()
{
	m_voltsPerAmp = kDefaultVoltsPerAmp;
	m_centrePosition = kDefaultCentrePosition;
	m_analog->SetAverageBits(kAverageBits);
	m_analog->SetOversampleBits(kOversampleBits);
	float sampleRate = kSamplesPerSecond * 
		(1 << (kAverageBits + kOversampleBits));
	m_analog->GetModule()->SetSampleRate(sampleRate);
}

/**
 * CurrentSensor constructor given a slot and a channel.
 * 
 * @param slot The cRIO slot for the analog module the Current Sensor is connected to.
 * @param channel The analog channel the Current Sensor is connected to.
 */
CurrentSensor::CurrentSensor(UINT32 slot, UINT32 channel)
{
	m_analog = new AnalogChannel(slot, channel);
	m_channelAllocated = true;
	InitCurrentSensor();
}

/**
 * CurrentSensor constructor with only a channel.
 * 
 * Use the default analog module slot.
 * 
 * @param channel The analog channel the Current Sensor is connected to.
 */
CurrentSensor::CurrentSensor(UINT32 channel)
{
	m_analog = new AnalogChannel(channel);
	m_channelAllocated = true;
	InitCurrentSensor();
}

/**
 * CurrentSensor constructor with a precreated analog channel object.
 * Use this constructor when the analog channel needs to be shared. There
 * is no reference counting when an AnalogChannel is passed to the Current Sensor.
 * @param channel The AnalogChannel object that the Current Sensor is connected to.
 */
CurrentSensor::CurrentSensor(AnalogChannel *channel)
{
	m_analog = channel;
	m_channelAllocated = false;
	if (channel == NULL)
	{
		wpi_fatal(NullParameter);
	}
	else
	{
		InitCurrentSensor();
	}
}

CurrentSensor::CurrentSensor(AnalogChannel &channel)
{
	m_analog = &channel;
	m_channelAllocated = false;
	InitCurrentSensor();
}

/**
 * Delete (free) the accumulator and the analog components used for the Current Sensor.
 */
CurrentSensor::~CurrentSensor()
{
	if (m_channelAllocated)
		delete m_analog;
}

/**
 * Return the actual current from the current sensors.
 * 
 * @return the current in amps.
 */
float CurrentSensor::GetCurrent( void )
{
	float result=m_analog->GetAverageVoltage()-m_centrePosition;
	result /= m_voltsPerAmp;
	return result;
}

/**
 * Set the Current Sensor sensitivity.
 * This takes the number of volts/amp sensitivity of the Current Sensor and uses it in subsequent
 * calculations to allow the code to work with multiple Current Sensors.
 * 
 * @param voltsPerAmpThe voltage that represents one amp for the current sensor.
 */
void CurrentSensor::SetSensitivity( float voltsPerAmp )
{
	m_voltsPerAmp = voltsPerAmp;
}

/**
 * Set the Current Sensor centre position.
 * This sets the value that represents zero amps from the sensor.
 * 
 * @param centrePosition The value that represents zero amps from the current sensor.
 */
void CurrentSensor::SetCentrePosition( float centrePosition )
{
	m_centrePosition = centrePosition;
}
