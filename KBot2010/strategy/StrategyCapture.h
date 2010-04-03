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
    
	bool BallCaptured();
	
	// if we lose the ball, keep track of how many cycles it was
	int m_nLostCounter;
	
	// if we lose the ball close in, keep track of how many cycles it was
	int m_nNearCounter;
	
	// did we last see the ball with the near or the far sensor?
	bool m_bFarLast;
	
	// number of times gate sensor has triggered
	int m_nGateCounter; 
	
	float  kfDriveForward;	// % voltage to drive forward
	float  kfFarTurn;		// % voltage for turn when far away
	int  knLostSweep;		// half second sweep
	int  knNearMax;		// one second to get close after losing near sensor

	int CAPTURE_MAX;
	
	int m_nTime1;
	
};

#endif
