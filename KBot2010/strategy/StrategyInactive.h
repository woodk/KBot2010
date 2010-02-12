#ifndef STRATEGY_INACTIVE_H
#define STRATEGY_INACTIVE_H

#include "Strategy.h"

/*
Inactive is the terminal state of the automatic control
system, and once it goes inactive it stays in active
forever.
*/
class StrategyInactive : public Strategy
{
public:

    // Constructor initializes object
	StrategyInactive(KBot* kbot);

    // Destructor cleans up
    ~StrategyInactive();

    // Just returns inactive as new state, always
    eState apply();
    
    void init();

private:
	bool m_done;

};


#endif
