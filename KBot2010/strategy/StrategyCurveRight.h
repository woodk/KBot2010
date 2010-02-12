#ifndef STRATEGY_CURVE_RIGHT_H
#define STRATEGY_CURVE_RIGHT_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to curve right, looking for targets
*/
class StrategyCurveRight : public Strategy
{
public:

    // Constructor initializes object
	StrategyCurveRight(KBot* kbot);

    // Destructor cleans up
    ~StrategyCurveRight();

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
