#include "StrategyShoot.h"

static const int KICK_TIMES = 10;

/*
Constructor initalizes object

*/
StrategyShoot::StrategyShoot(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_nKickCounter = 0;
	m_nTimesKicked = 0;
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
    if (m_nTimesKicked < KICK_TIMES)
    {
    	m_kbot->getKicker()->Kick();
    	++m_nTimesKicked;
    }
    else
    {
    	m_nTimesKicked = 0;
	   	if (m_nKickCounter < (int)(m_vecNextState.size()))
    	{
    		nNewState = m_vecNextState[m_nKickCounter];
        	++m_nKickCounter;
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
	m_nTimesKicked = 0;
}

