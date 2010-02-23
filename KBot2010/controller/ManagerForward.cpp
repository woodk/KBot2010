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
	// Forward gets 1 ball.
	switch(kbot->getAutoPattern())
	{
	case 0:	// Kick ball in position 9
		printf("***************************************Forward pattern 0\n");
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0, 72.0-10.42-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-5.95, 10.42);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(35.67,62.54);	// Get out of the way
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		break;
	case 1:	// Kick ball in position 6
		printf("***************************************Forward pattern 1\n");
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0,9.0*12-9.56-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-7.26,9.56);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(21.77,28.68);	// Get out of the way
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		break;
	case 2:	// Kick ball in position 3
		printf("***************************************Forward pattern 2\n");
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0,12.0*12-7.92-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-9.02,7.92);
	
		// No more moves; remove knProgrammedMove2 & 3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		break;
	case 3:	// Kick ball in position 2
	default:
		printf("***************************************Forward pattern 3\n");
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0,12.0*12-10.42-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-5.95,10.42);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(31.27,-17.84);	// Get out of the way
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
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
