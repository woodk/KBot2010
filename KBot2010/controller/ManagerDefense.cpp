#include "ManagerDefense.h"

#include "StrategyAim.h"
#include "StrategyCapture.h"
#include "StrategyProgrammedMove.h"
#include "StrategySearch.h"
#include "StrategyShoot.h"
#include "StrategyCircle.h"
#include "StrategyBackUp.h"
#include "StrategyInactive.h"

#define ROBOT_LENGTH	41	// inches

/*!
Constructor sets up initial strategy
*/
ManagerDefense::ManagerDefense(KBot *kbot) : RobotManager(kbot)
{
	setStartState(knProgrammedMove1);
	
	StrategyProgrammedMove* pMove1 = (StrategyProgrammedMove*)(m_lstStrategy[knProgrammedMove1]);
	pMove1->setState(knProgrammedMove1);
	StrategyProgrammedMove* pMove2 = (StrategyProgrammedMove*)(m_lstStrategy[knProgrammedMove2]);
	pMove2->setState(knProgrammedMove2);
	StrategyProgrammedMove* pMove3 = (StrategyProgrammedMove*)(m_lstStrategy[knProgrammedMove3]);
	pMove3->setState(knProgrammedMove3);
	((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().push_back(knProgrammedMove2);
	((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().push_back(knProgrammedMove3);
	// Ball positions:
	//		1	2	3
	//		4	5	6
	//		7	8	9
	switch(kbot->getAutoPattern())
	{
	case 0:	// Kick balls in positions 9, 5 then 1
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(3.3*12,-2.7*12);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(3.3*12,-2.7*12);

		// No more moves; remove knProgrammedMove4 from the list.
		//(StrategyShoot*)(m_lstStrategy[knShoot])->getNextState().pop_back();
		break;
	case 1:	// Kick balls in positions 7, 5 then 3
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(2.4*12,3.3*12);
		pMove2->addPointToPath(-0.9*12,0.3*12);	// angle towards goal
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(2.4*12,3.3*12);
		pMove3->addPointToPath(-0.9*12,0.3*12);	// angle towards goal
		break;
	case 2:	// Kick balls in positions 9, 6 then 3
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(5.7*12,0.3*12);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(5.7*12,0.3*12);
		break;
	case 3:	// Kick balls in positions 7, 4 then 1
	default:
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(6.0*12,0);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(6.0*12,0);
	}
		
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
