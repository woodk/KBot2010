#ifndef STRATEGY_GO_ALONG_END_H
#define STRATEGY_GO_ALONG_END_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategyGoAlongEnd : public Strategy
{
public:

    // Constructor initializes object
	StrategyGoAlongEnd(KBot* kbot);

    // Destructor cleans up
    ~StrategyGoAlongEnd();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:
    
    Camera *m_camera;
    //Pickup *m_pickup;

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepMoving();
    // Look for a target
	bool TargetInSight();
    
};

#endif
