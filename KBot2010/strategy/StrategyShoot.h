#ifndef STRATEGY_SHOOT_H
#define STRATEGY_SHOOT_H

#include "Strategy.h"

#include <vector>

/*
The strategy used to spin on the spot, looking for targets.
*/
class StrategyShoot : public Strategy
{
public:

    // Constructor initializes object
	StrategyShoot(KBot* kbot);

    // Destructor cleans up
    ~StrategyShoot();

    // run away!
    eState apply();
    
    void init();
    
    std::vector<eState> getNextStates() {return m_vecNextState;}

private:
    
	bool TargetInSight();
	
	std::vector<eState> m_vecNextState;
	
	int m_nKickCounter;
    
};

#endif
