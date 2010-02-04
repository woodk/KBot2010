#ifndef STRATEGY_SEARCH_H
#define STRATEGY_SEARCH_H

#include "Strategy.h"

/*
The strategy used to search for balls
*/
class StrategySearch : public Strategy
{
public:

    // Constructor initializes object
	StrategySearch(KBot* kbot);

    // Destructor cleans up
    ~StrategySearch();

    // run away!
    eState apply();
    
    void init();

private:
    
	bool BallInSight();
    
};

#endif
