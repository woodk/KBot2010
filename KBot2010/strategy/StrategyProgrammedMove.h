#ifndef STRATEGY_PROGRAMMED_MOVE_H
#define STRATEGY_PROGRAMMED_MOVE_H

#include "Strategy.h"

#include <vector>

class KbotPID;

/*
The strategy used to execute a progammed move
*/
class StrategyProgrammedMove : public Strategy
{
public:

    // Constructor initializes object
	StrategyProgrammedMove(KBot* kbot);

    // Destructor cleans up
    ~StrategyProgrammedMove();

    // run away!
    eState apply();
    
    // set the path as a series of (x,y) points in meters
    void addPointToPath(float fX, float fY) {m_vecX.push_back(fX); m_vecY.push_back(fY);}
    
    // clear the path
    void clearPath() {m_vecX.clear(); m_vecY.clear();}
    
    // set the current state
    void setState(eState nState) {m_nState = nState;}
    
    // set the state that follows this one
    void setNextState(eState nNextState) {m_nNextState = nNextState;}
    
    void init();

private:

	// the lists of points
	std::vector<float> m_vecX;
	std::vector<float> m_vecY;
	
	// the current index into the path
	int m_nPathIndex;
	
	// The current x-position (m)
	float m_fX;
	float m_fY;
	
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
	
	// PID control for distance--NOT USED
	KbotPID* m_pidDistance;
	
	// current forward speed
	float m_fForwardSpeed;
	
	// current angular speed
	float m_fAngularSpeed;
	
	// flag to tell if people are turning
	bool m_bTurning;

	// flag to tell if this is first call to programmed move
	bool m_bFirstCall;

};

#endif
