#ifndef STRATEGY_GO_ALONG_EDGE_H
#define STRATEGY_GO_ALONG_EDGE_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategyGoAlongEdge : public Strategy
{
public:

    // Constructor initializes object
	StrategyGoAlongEdge(KBot* kbot);

    // Destructor cleans up
    ~StrategyGoAlongEdge();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepMoving();
    // Look for a target
	bool TargetInSight();
};

#endif
