#ifndef STRATEGY_SPIN_H
#define STRATEGY_SPIN_H

#include "Strategy.h"

/*
The strategy capture a ball once we have seen one
*/
class StrategyCapture : public Strategy
{
public:

    // Constructor initializes object
	StrategyCapture(KBot* kbot);

    // Destructor cleans up
    ~StrategyCapture();

    // run away!
    eState apply();
    
    void init();

private:
    
	bool TargetCaptured();
};

#endif
