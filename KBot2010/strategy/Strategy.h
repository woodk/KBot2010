#ifndef STRATEGY_H
#define STRATEGY_H

#include "State.h"
#include "KBot.h"

/* Represents a control strategy used by the robot
 * 
 * Every state of the robot has one or more strategies
 * associated with it.  This class is the base class for
 * those strategies, allowing the manager to contain a
 * generic pointer to whatever strategy is relevant at 
 * the time.
*/
class Strategy
{
public:

	// Constructor sets bot pointer
	Strategy(KBot* kbot);
	
    // Pure virtual classes should have explicity declared virtual destructor
    virtual ~Strategy() {;}

    // This method is actually called from the framework to apply hysteresis
    eState execute();
    
    // Pure virtual method that applies strategy and returns new state
    virtual eState apply() = 0;

    virtual void init() {;}

protected:
	
	// The bot objects (represents the hardware)
	KBot* m_kbot;
    RobotDrive *m_robotDrive;
	Gyro *m_gyro;
	KbotPID *m_gyroDriveCtrl;
	
	// counter for hysteresis--counts number of times we have been in this state
	int m_nCurrentStateCounter;
	
	// counter for hysteresis--counts number of times we have requested a new state
	int m_nChangeStateCounter;
	
	// keep track of current state to prevent rapid swapping
	eState m_nCurrentState;
	
	// keep track of previous state to prevent rapid swapping
	eState m_nPreviousState;
	
	// the basic number of counts required before state change
	int m_nMinimumStateCount;
	
	// the number of times we have to try to change in a row before permited
	int m_nMinimumChangeCount;

};

#endif
