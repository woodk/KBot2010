#ifndef STRATEGY_S_CURVE_H
#define STRATEGY_S_CURVE_H

#include "Strategy.h"
#include "RobotDrive.h"

/*
The strategy used to go straight, looking for targets
*/
class StrategySCurve : public Strategy
{
public:

    // Constructor initializes object
	StrategySCurve(KBot* kbot);

    // Destructor cleans up
    ~StrategySCurve();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();
    
    void init();


private:
    //Pickup *m_pickup;

    // The number of times this strategy has been called
    int m_nCallCount;
    
    float m_curveDir;

    // Check if we should keep going
    bool getKeepMoving();
    // Look for a target
	bool TargetInSight();
    
};

#endif
