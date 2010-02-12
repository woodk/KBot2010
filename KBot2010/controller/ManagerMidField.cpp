#include "ManagerMidField.h"

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
ManagerMidField::ManagerMidField(KBot *kbot) : RobotManager(kbot)
{
	setStartState(knProgrammedMove1);
	reset();
	
    // TODO: set parameters on programmed moves
    
}

/*!
Destructor does nothing
*/
ManagerMidField::~ManagerMidField()
{
}

/*!
If the manager is active then passes control on to the
current strategy, and changes strategies for the next
step if requested by the current strategy.
*/
void ManagerMidField::onClock(bool bActive)
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
