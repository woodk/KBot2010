#ifndef STRATEGY_AIM_H
#define STRATEGY_AIM_H

#include "Strategy.h"

/*
The strategy used to aim.
*/
class StrategyAim : public Strategy
{
public:

    // Constructor initializes object
	StrategyAim(KBot* kbot);

    // Destructor cleans up
    ~StrategyAim();

    // run away!
    eState apply();
    
    void init();

private:
    
	bool TargetLocked();
    
};

#endif
