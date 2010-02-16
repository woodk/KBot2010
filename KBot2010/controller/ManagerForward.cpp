#include "ManagerForward.h"

#include "StrategyAim.h"
#include "StrategyCapture.h"
#include "StrategyProgrammedMove.h"
#include "StrategySearch.h"
#include "StrategyShoot.h"
#include "StrategyCircle.h"
#include "StrategyBackUp.h"
#include "StrategyInactive.h"

#define ROBOT_LENGTH	41	// inches
//TODO: find proper value ...
#define HALF_RAMP_WIDTH	18	// inches

/*!
Constructor sets up initial strategy
*/
ManagerForward::ManagerForward(KBot *kbot) : RobotManager(kbot)
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
	// Defense gets 3 balls.
	switch(kbot->getAutoPattern())
	{
	case 0:	// Kick balls in positions 6 then 3
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(9.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(2.8*12,1.0*12);
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		break;
	case 1:	// Kick balls in positions 4 then 1
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(9.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(3.0*12,0.0);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(0.0,6.0*12);	// get out of the way
		break;
	case 2:	// Kick balls in positions 9 then 1
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(5.0*12,6.0*12);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(1.0*12,0.0*12); // line up with goal
		break;
	case 3:	// Kick balls in positions 7 then 3
	default:
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(6.0*12,0);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(6.0*12,0);
	}
	
	reset();
		
    // TODO: set parameters on programmed moves properly
    
}

/*!
Destructor does nothing
*/
ManagerForward::~ManagerForward()
{
}

/*!
If the manager is active then passes control on to the
current strategy, and changes strategies for the next
step if requested by the current strategy.
*/
void ManagerForward::onClock(bool bActive)
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
