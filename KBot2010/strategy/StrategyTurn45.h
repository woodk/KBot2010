#ifndef STRATEGY_TURN_45_H
#define STRATEGY_TURN_45_H

#include "Strategy.h"
#include "KBotDrive.h"

/*
The strategy used to turn 45 degrees left or right
*/
class StrategyTurn45 : public Strategy
{
public:

    // Constructor initializes object
	StrategyTurn45(KBot* kbot);

    // Destructor cleans up
    ~StrategyTurn45();

    // Returns the next state: Spin if time expires or Track if a target is spotted
    eState apply();

    void setNextState(eState next);
   
    void init();

private:

    // The number of times this strategy has been called
    int m_nCallCount;

    // Check if we should keep going
    bool getKeepTurning();
    
    eState m_nextState;
};

#endif
