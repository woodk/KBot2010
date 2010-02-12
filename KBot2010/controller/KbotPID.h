#ifndef KBOTPID_H_
#define KBOTPID_H_

class KbotPID
{
public:
	KbotPID(float p = 0.0, float i = 0.0, float d = 0.0, float epsilon = 0.0);
	
	void setConstants(float p, float i, float d);
	void setErrorEpsilon(float epsilon);
	void setErrorIncrement(float inc);
	void setDesiredValue(float val);
	void setMaxOutput(float max);
	void setMinOutput(float min);
	void setDeadBand(float deadband);
	void resetErrorSum(void);
		
	float calcPID(float current);
	
	bool isDone(void);
	void setMinDoneCycles(int n);
	
private:
	float m_p;   // P coefficient
	float m_i;   // I coefficient
	float m_d;   // D coefficient

	float m_desiredValue; // Desired value
	float m_previousValue; // Value at last call
	float m_errorSum; // Sum of previous errors (for I calculation)
	float m_errorIncrement; // Max increment to error sum each call
	float m_errorEpsilon; // Allowable error in determining when done
	
	bool m_firstCycle; // Flag for first cycle
	float m_maxOutput; // Ceiling on calculation output
	float m_minOutput; // Floor on calculation output
	float m_deadBand;  // Deadband for the output calculation

	int m_minCycleCount; // Minimum number of cycles in epsilon range to be done
	int m_cycleCount; // Current number of cycles in epsilon range
};

#endif // KBOTPID_H_
