#ifndef STRATEGY_BACKUP_H
#define STRATEGY_BACKUP_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategyBackUp : public Strategy
{
public:

    // Constructor initializes object
	StrategyBackUp(KBot* kbot);

    // Destructor cleans up
    ~StrategyBackUp();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:

	Shooter *m_shooter;
	
    // the time we should run for in seconds
    float m_fRunTimeInSeconds;

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepMoving();
    
};

#endif
