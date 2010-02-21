#include "StrategySearch.h"

/*
Constructor initalizes object

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
  		m_robotDrive->ArcadeDrive(0.0, 0.0, false);
    }
    else	// just spin to search (safe: will not cross line)
    {
  		m_robotDrive->ArcadeDrive(0.0, -0.5, false);
    }

    return nNewState;
}

void StrategySearch::init()
{
    printf("Search state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_SPIN_STATE,true);
}

/* Check ultrasound to see if ball is in sight */
bool StrategySearch::BallInSight()
{
	return (m_kbot->getNearUltrasoundState() || m_kbot->getFarUltrasoundState());
}
