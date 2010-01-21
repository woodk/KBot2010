#include "KbotPID.h"
#include "stdio.h"

// Based on Simbotics SimPID.  Thanks!

/**
 * Initializes the KbotPID object. All parameters default to 0.
 */
KbotPID::KbotPID(float p, float i, float d, float epsilon)
{
	m_p = p;
	m_i = i;
	m_d = d;

	m_errorEpsilon = epsilon;
	m_desiredValue = 0.0; // Default to 0, set later by the user
	m_firstCycle = true;
	m_maxOutput = 1.0; // Default to full range
	m_minOutput = -1.0; // Default to full range
	m_deadBand = 0.0;  // Default to no deadband
	m_errorIncrement = 0.05;

	m_cycleCount = 0;
	m_minCycleCount = 10; // Default
}

/**
 * Sets the PID constants to new values.
 */
void KbotPID::setConstants(float p, float i, float d)
{
	m_p = p;
	m_i = i;
	m_d = d;
	
}

/**
 * Sets the allowable error range away from the desired value.
 */
void KbotPID::setErrorEpsilon(float epsilon)
{
	m_errorEpsilon = epsilon;
}

/**
 * Sets the maximum increment to the error sum used in the I component
 * calculation.
 * This defaults to 1 in the constructor, which has worked well for 1114 the
 * past few years.
 */
void KbotPID::setErrorIncrement(float inc)
{
	m_errorIncrement = inc;
}

/**
 * Sets the desired value.
 */
void KbotPID::setDesiredValue(float val)
{
	m_desiredValue = val;
}
	
/**
 * Sets the ceiling for the output of the calculation.
 * This defaults to 1.0 (full output). Values should be between 0.0 and 1.0.
 */
void KbotPID::setMaxOutput(float max)
{	if(max >= 0.0 && max <= 1.0)
	{
		m_maxOutput = max;
	}
}

/**
 * Sets the floor for the output of the calculation.
 * This defaults to -1.0 (full output). Values should be between -1.0 and 0.0.
 */
void KbotPID::setMinOutput(float min)
{	if(min >= -1.0 && min <= 0.0)
	{
		m_minOutput = min;
	}
}

/**
 * Sets the deadband for the output calculation.
 * This defaults to 0.0 (no deadband). Values must be between -1.0 and 1.0
 *  (likely between -0.2 amd 0.2).
 */
void KbotPID::setDeadBand(float deadBand)
{	if(deadBand >= -1.0 && deadBand <= 1.0)
	{
		m_deadBand = deadBand;
	}
}

/**
 * Resets the error sum back to zero.
 */
void KbotPID::resetErrorSum(void)
{
	m_errorSum = 0;
}

/**
 * Calculates the PID output based on the current value.
 * PID constants and desired value should be set before calling this
 * function.
 */
float KbotPID::calcPID(float currentValue)
{	
	// Initialize all components to 0.0 to start.
	float pVal = 0.0;
	float iVal = 0.0;
	float dVal = 0.0;
		
	// Don't apply D the first time through.
	if(m_firstCycle)
	{	
		m_previousValue = currentValue;  // Effective velocity of 0
		m_firstCycle = false;
	}
	
	// Calculate P Component.
	float error = m_desiredValue - currentValue;
	pVal = m_p * (float)error;
	
	// Calculate I Component.
	// Error is positive and outside the epsilon band.
	if(error >= m_errorEpsilon)
	{	
		// Check if epsilon was pushing in the wrong direction.
		if(m_errorSum < 0)
		{
			// If we are fighting away from the point, reset the error.
			m_errorSum = 0;
		}
		if(error < m_errorIncrement)
		{
			// If the error is smaller than the max increment amount, add it.
			m_errorSum += error;
		}
		else 
		{
			// Otherwise, add the maximum increment per cycle.
			m_errorSum += m_errorIncrement;      
		}
	}
	// Error is negative and outside the epsilon band.
	else if(error <= -m_errorEpsilon)
	{	
		if(m_errorSum > 0)
		{
			// If we are fighting away from the point, reset the error.
			m_errorSum = 0;
		}
		if(error > -m_errorIncrement)
		{
			// If the error is smaller than the max increment amount, add it.
			m_errorSum += error; // Error is negative
		}
		else
		{
			// Otherwise, subtract the maximum increment per cycle.
			m_errorSum -= m_errorIncrement;
		}
	}
	// Error is inside the epsilon band. 
	else
	{
		m_errorSum = 0;
	}
	iVal = m_i * (float)m_errorSum;
	
	// Calculate D Component.
	float velocity = currentValue - m_previousValue;
	dVal = m_d * velocity;

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
	else if (output > -m_deadBand && output < m_deadBand)
	{
		output = 0.0;
	}
	//printf("err=%f P=%f esum=%f I=%f v=%f D=%f out=%f\n",error,pVal,m_errorSum,iVal,velocity,-dVal,output);
	// Save the current value for next cycle's D calculation.
	m_previousValue = currentValue;
	
	return output;
}

/**
 * Sets the minimum number of cycles the value must be in the epsilon range
 * before the system is considered stable.
 */
void KbotPID::setMinDoneCycles(int n)
{
	m_minCycleCount = n;
}

/**
 * Returns true if the last input was within the epsilon range of the
 * destination value, and the system is stable.
 */
bool KbotPID::isDone(void)
{	
	if (m_previousValue <= m_desiredValue + m_errorEpsilon
			&& m_previousValue >= m_desiredValue - m_errorEpsilon
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
