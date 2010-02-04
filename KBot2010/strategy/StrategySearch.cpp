#include "StrategySearch.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategySearch::StrategySearch(KBot* kbot) : Strategy(kbot)
{
}

/*
Destructor cleans up
*/
StrategySearch::~StrategySearch()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategySearch::apply()
{
    eState nNewState = knSearch;    // assume we will keep running

    // Keep turning until we find a target
    if (BallInSight())
    {
        // start tracking the target
        nNewState = knCapture;
    }
    else
    {
    	// TODO: driving logic here
  		//m_robotDrive->ArcadeDrive(-0.8, -0.9, false);
    }

    return nNewState;
}

void StrategySearch::init()
{
    printf("Search state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_SPIN_STATE,true);
}

/* Check camera to see if we can see any targets */
bool StrategySearch::BallInSight()
{
	// TODO: set up to operate with new camera
	return false;
}
