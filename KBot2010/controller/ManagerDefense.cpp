#include "ManagerDefense.h"

#include "StrategyAim.h"
#include "StrategyCapture.h"
#include "StrategyProgrammedMove.h"
#include "StrategySearch.h"
#include "StrategyShoot.h"
#include "StrategyCircle.h"
#include "StrategyBackUp.h"
#include "StrategyInactive.h"

/*!
Constructor sets up initial strategy
*/
ManagerDefense::ManagerDefense(KBot *kbot) : RobotManager(kbot)
{
	setStartState(knProgrammedMove1);
	
	StrategyProgrammedMove* pMove1 = (StrategyProgrammedMove*)(m_lstStrategy[knProgrammedMove1]);
	pMove1->setState(knProgrammedMove1);
	pMove1->addPointToPath(0,0);
	pMove1->addPointToPath(100,0);
	pMove1->addPointToPath(0,100);
	
	reset();
	
    // TODO: set parameters on programmed move properly
	
    
}

/*!
Destructor does nothing
*/
ManagerDefense::~ManagerDefense()
{
}

/*!
If the manager is active then passes control on to the
current strategy, and changes strategies for the next
step if requested by the current strategy.
*/
void ManagerDefense::onClock(bool bActive)
{
    if (bActive)
    {
        eState nCurrentState = getState();  // get the current state and apply strategy
        eState nNewState = m_lstStrategy[nCurrentState]->execute();
        setState(nNewState);
        if (nNewState != nCurrentState)
        {
            m_lstStrategy[nNewState]->init();
        }
    }
}
