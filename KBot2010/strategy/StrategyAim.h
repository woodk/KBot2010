#ifndef STRATEGY_AIM_H
#define STRATEGY_AIM_H

#include "Strategy.h"
#include <vector>

// Camera point including x-coordinate and timestamp
struct camPoint {
	float m_x;
	float m_t;
	camPoint(float x, float t)
	{
		m_x=x;
		m_t=t;
	}
};

/*
The strategy used to aim.
*/
class StrategyAim : public Strategy
{
public:

    // Constructor initializes object
	StrategyAim(KBot* kbot);

    // Destructor cleans up
    ~StrategyAim();

    // run away!
    eState apply();
    
    void init();

private:
    
	bool TargetLocked();

	// The current direction (angle in radians from initial direction)
	float m_fAngle;
	
	// The desired direction
	float m_fDirection;
	
	// the tolerance on angles
	float m_fAngleTolerance;
	
	// The time of the last call in ms
	float m_fLastTime;
	
	// The distance along the current segment
	float m_fDistance;
	
	// The length of the current segment
	float m_fLength;
	
	bool MoveComplete();
	
	// the state to change into when the move is complete
	eState m_nNextState;
	
	// the current state
	eState m_nState;
	
	// PID control for angle
	KbotPID* m_pidAngle;
	
	// PID control for distance
	KbotPID* m_pidDistance;
	
	// current forward speed
	float m_fForwardSpeed;
	
	// current angular speed
	float m_fAngularSpeed;
	
	// list of camera points
	std::vector<camPoint> m_vecPoints;
	// count number of points added to list
	int m_pointCounter;
	
};

#endif
