#ifndef STRATEGY_PROGRAMMED_MOVE_H
#define STRATEGY_PROGRAMMED_MOVE_H

#include "Strategy.h"

/*
The strategy used to execute a progammed move
*/
class StrategyProgrammedMove : public Strategy
{
public:

    // Constructor initializes object
	StrategyProgrammedMove(KBot* kbot);

    // Destructor cleans up
    ~StrategyProgrammedMove();

    // run away!
    eState apply();
    
    void init();

private:

	bool MoveComplete();
	
};

#endif
