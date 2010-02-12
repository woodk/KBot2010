#ifndef KBOTVPID_H_
#define KBOTVPID_H_

class KbotVPID
{
public:
	KbotVPID(float p = 0.0, float i = 0.0, float d = 0.0, float epsilon = 0.0);
	
	void setConstants(float p, float i, float d);
	void setErrorEpsilon(float epsilon);
	void setErrorIncrement(float inc);
	void setDesiredValue(float val);
	void setMaxOutput(float max);
	void setMinOutput(float min);
	void resetErrorSum(void);
		
	float calcPID(float current);
	
	bool isDone(void);
	void setMinDoneCycles(int n);
	
private:
	float m_Kp;   // P coefficient
	float m_Ki;   // I coefficient
	float m_Kd;   // D coefficient

	float m_desiredValue; // Desired value
	float m_valueT_1;  // Value at last cycle
	float m_valueT_2;  // Value at cycle before last
	float m_errorT_1;  // Error at last cycle
	float m_errorSum; // Sum of previous errors (for I calculation)
	float m_errorIncrement; // Max increment to error sum each call
	float m_errorEpsilon; // Allowable error in determining when done
	
	bool m_firstCycle; // Flag for first cycle
	float m_maxOutput; // Ceiling on calculation output
	float m_minOutput; // Floor on calculation output

	int m_minCycleCount; // Minimum number of cycles in epsilon range to be done
	int m_cycleCount; // Current number of cycles in epsilon range
};

#endif // KBOTVPID_H_
