#include "StrategyShoot.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyShoot::StrategyShoot(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_nKickCounter = 0;
}

/*
Destructor cleans up
*/
StrategyShoot::~StrategyShoot()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyShoot::apply()
{
    eState nNewState = knShoot;    // assume we will keep running

   	// FIRE!
    if (m_kbot->getKicker()->getIsReady())
    {
    	m_kbot->getKicker()->Kick();
    	++m_nKickCounter;
    	if (m_nKickCounter < (int)(m_vecNextState.size()))
    	{
    		nNewState = m_vecNextState[m_nKickCounter];
    	}
    	else
    	{
    		nNewState = knSearch;    		
    	}
    }
    
    return nNewState;
}

void StrategyShoot::init()
{
    printf("Shoot state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
    //m_robotDrive->setTorque(120);
}

