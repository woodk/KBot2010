#ifndef ROBOT_MANAGER_H
#define ROBOT_MANAGER_H

#include "State.h"
#include "KBot.h"

class Strategy;
class KBot;

/* The manager that guides the robot
 * This is the master AI that guides the robot in automous mode
 */
class RobotManager
{
public:

    // Constructor sets up initial state
	RobotManager(KBot *kbot);

    // Destructor cleans up
    ~RobotManager();

    // This method is called by the handler for the 200 Hz clock
    void onClock(bool bActive);

    // Accessor for state
    eState getState() {return m_nState;}

    // Accessor for state
    void setState(eState nState) {m_nState = nState;}
    void reset();

private:

    // The current state
    eState m_nState;
    
    eState m_initialState;
    
    KBot *m_kbot;
    DriverStation *m_driverStation;

    // the list of available strategies, indexed by state.  Initial is max state value.
    Strategy* m_lstStrategy[knInitial];
};

#endif
