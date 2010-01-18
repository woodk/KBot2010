#ifndef STRATEGY_SPIN_H
#define STRATEGY_SPIN_H

#include "Strategy.h"

/*
The strategy used to spin on the spot, looking for targets.
*/
class StrategySpin : public Strategy
{
public:

    // Constructor initializes object
	StrategySpin(KBot* kbot);

    // Destructor cleans up
    ~StrategySpin();

    // run away!
    eState apply();
    
    void init();

private:
    
    Camera *m_camera;
    Shooter *m_shooter;
    
	bool TargetInSight();
    
};

#endif
