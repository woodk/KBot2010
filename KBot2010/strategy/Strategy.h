#ifndef STRATEGY_H
#define STRATEGY_H

#include "State.h"
#include "KBot.h"

class KBot;

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
	Strategy(KBot* kbot) : m_kbot(kbot) {;}
	
    // Pure virtual classes should have explicity declared virtual destructor
    virtual ~Strategy() {;}

    // Pure virtual method that applies strategy and returns new state
    virtual eState apply() = 0;

    virtual void init() {;}

protected:
	
	// The bot objects (represents the hardware)
	KBot* m_kbot;
    RobotDrive *m_robotDrive;
	Gyro *m_gyro;
	KbotPID *m_gyroDriveCtrl;

};

#endif
