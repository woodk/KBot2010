#ifndef STRATEGY_GO_STRAIGHT_H
#define STRATEGY_GO_STRAIGHT_H

#include "Strategy.h"
#include "KBotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategyGoStraight : public Strategy
{
public:

    // Constructor initializes object
	StrategyGoStraight(KBot* kbot);

    // Destructor cleans up
    ~StrategyGoStraight();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:

    //Pickup *m_pickup;

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepMoving();
    // Look for a target
	bool TargetInSight();
    
};

#endif
