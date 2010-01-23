#ifndef STRATEGY_CIRCLE_H
#define STRATEGY_CIRCLE_H

#include "Strategy.h"

/*
The strategy used to circle, but not looking for targets.
*/
class StrategyCircle : public Strategy
{
public:

    // Constructor initializes object
	StrategyCircle(KBot* kbot);

    // Destructor cleans up
    ~StrategyCircle();

    // run away!
    eState apply();
    
    void init();

private:
	
};

#endif
