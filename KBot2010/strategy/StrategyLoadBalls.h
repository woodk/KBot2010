#ifndef STRATEGY_LOAD_BALLS_H
#define STRATEGY_LOAD_BALLS_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategyLoadBalls : public Strategy
{
public:

    // Constructor initializes object
	StrategyLoadBalls(KBot* kbot);

    // Destructor cleans up
    ~StrategyLoadBalls();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:
    
    Pickup *m_pickup;

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepMoving();
    // Look for a target
	//bool TargetInSight();
    
};

#endif
