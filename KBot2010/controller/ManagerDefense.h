#ifndef MANAGER_DEFENSE_H
#define MANAGER_DEFENSE_H

#include "State.h"
#include "KBot.h"
#include "RobotManager.h"

class Strategy;
class KBot;

/* The manager that guides the robot
 * This is the master AI that guides the robot in automous mode
 */
class ManagerDefense : public RobotManager
{
public:

    // Constructor sets up initial state
	ManagerDefense(KBot *kbot);

    // Destructor cleans up
    ~ManagerDefense();

    // This method is called by the handler for the 200 Hz clock
    void onClock(bool bActive);
    
private:
};

#endif
