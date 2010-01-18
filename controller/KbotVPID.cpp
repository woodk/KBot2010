#include "KbotVPID.h"

/**
 * Initializes the KbotVPID object.  Velocity based PID.
 * 
 * All parameters default to 0.
 */
KbotVPID::KbotVPID(float p, float i, float d, float epsilon)
{
	m_Kp = p;
	m_Ki = i;
	m_Kd = d;

	m_errorEpsilon = epsilon;
	m_desiredValue = 0.0; // Default to 0, set later by the user
	m_firstCycle = true;
	m_maxOutput = 1.0; // Default to full range
	m_minOutput = -1.0; // Default to full range
	m_errorIncrement = 1.0;

	m_cycleCount = 0;
	m_minCycleCount = 10; // Default
}

/**
 * Sets the PID constants to new values.
 */
void KbotVPID::setConstants(float p, float i, float d)
{
	m_Kp = p;
	m_Ki = i;
	m_Kd = d;
	
}

/**
 * Sets the allowable error range away from the desired value.
 */
void KbotVPID::setErrorEpsilon(float epsilon)
{
	m_errorEpsilon = epsilon;
}

/**
 * Sets the maximum increment to the error sum used in the I component
 * calculation.
 * This defaults to 1 in the constructor, which has worked well for 1114 the
 * past few years.
 */
void KbotVPID::setErrorIncrement(float inc)
{
	m_errorIncrement = inc;
}

/**
 * Sets the desired value.
 */
void KbotVPID::setDesiredValue(float val)
{
	m_desiredValue = val;
}
	
/**
 * Sets the ceiling for the output of the calculation.
 * This defaults to 1.0 (full output). Values should be between 0.0 and 1.0.
 */
void KbotVPID::setMaxOutput(float max)
{	if(max >= 0.0 && max <= 1.0)
	{
		m_maxOutput = max;
	}
}

/**
 * Sets the floor for the output of the calculation.
 * This defaults to -1.0 (full output). Values should be between -1.0 and 0.0.
 */
void KbotVPID::setMinOutput(float min)
{	if(min >= -1.0 && min <= 0.0)
	{
		m_minOutput = min;
	}
}

/**
 * Resets the error sum back to zero.
 */
void KbotVPID::resetErrorSum(void)
{
	m_errorSum = 0;
}

/**
 * Calculates the PID output based on the current value.
 * PID constants and desired value should be set before calling this
 * function.
 */
float KbotVPID::calcPID(float currentValue)
{	
	// Initialize all components to 0.0 to start.
	float pVal = 0.0;
	float iVal = 0.0;
	float dVal = 0.0;
	float error;
	
	// Don't apply D the first time through.
	if(m_firstCycle)
	{	
		m_valueT_2 = m_valueT_1 = currentValue;
		m_errorT_1 = error = 0;  // Errors are 0
		m_firstCycle = false;
	}
	
	// Calculate difference of "P" Component.
	error = m_desiredValue - currentValue;
	pVal = m_Kp * (error - m_errorT_1);
	
	// Calculate difference of "I" Component.
	// Error is positive and outside the epsilon band.
	iVal = m_Ki * error;
	
	// Calculate difference of "D" Component.
	dVal = m_Kd * (currentValue - 2*m_valueT_1 + m_valueT_2);

	// Calculate and limit the ouput: Output = P + I - D
	float output = pVal + iVal - dVal;
	if(output > m_maxOutput)
	{
		output = m_maxOutput;
	}
	else if(output < m_minOutput)
	{
		output = m_minOutput;
	}
	
	// Save the current values for next cycle
	m_errorT_1 = error;
	m_valueT_2 = m_valueT_1;
	m_valueT_1 = currentValue;
	
	return output;
}

/**
 * Sets the minimum number of cycles the value must be in the epsilon range
 * before the system is considered stable.
 */
void KbotVPID::setMinDoneCycles(int n)
{
	m_minCycleCount = n;
}

/**
 * Returns true if the last input was within the epsilon range of the
 * destination value, and the system is stable.
 */
bool KbotVPID::isDone(void)
{	
	if (m_valueT_1 <= m_desiredValue + m_errorEpsilon
			&& m_valueT_1 >= m_desiredValue - m_errorEpsilon
			&& !m_firstCycle)
	{
		if(m_cycleCount >= m_minCycleCount)
		{
			return true;
		}
		else 
		{	
			m_cycleCount++;
		}
	}
	m_cycleCount = 0;
	return false;
}
