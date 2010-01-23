#ifndef STRATEGY_CURVE_LEFT_H
#define STRATEGY_CURVE_LEFT_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to curve left, looking for targets
*/
class StrategyCurveLeft : public Strategy
{
public:

    // Constructor initializes object
	StrategyCurveLeft(KBot* kbot);

    // Destructor cleans up
    ~StrategyCurveLeft();

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
