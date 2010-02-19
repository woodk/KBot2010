#include "ManagerMidField.h"

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
#define HALF_RAMP_WIDTH	13.75	// inches (37.5/2 - 5)  (5" is the overlap)

/*!
Constructor sets up initial strategy
*/
ManagerMidField::ManagerMidField(KBot *kbot) : RobotManager(kbot)
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
	// Midfield gets 2 balls.
	switch(kbot->getAutoPattern())
	{
	case 0:	// Kick balls in positions 6 then 3
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0,96.37-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-2.94,11.63);	// Angle towards target
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(9.84,22.63);
		pMove2->addPointToPath(-4.78,11.00);	// Angle towards target
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
		break;
	case 1:	// Kick balls in positions 4 then 1
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0, 9.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(0.0,36.0);
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(72.0,0.0);	// get out of the way
		break;
	case 2:	// Kick balls in positions 9 then 1
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(0.0,60.3-ROBOT_LENGTH-HALF_RAMP_WIDTH);
		pMove1->addPointToPath(-2.66,11.7);	// Angle towards target
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(-56.47,74.40);
		pMove2->addPointToPath(8.92,8.03);	// Angle towards target
	
		pMove3->addPointToPath(0,0);
		pMove3->addPointToPath(72.0,0.0); // get out of the way
		break;
	case 3:	// Kick balls in positions 7 then 3
	default:
		pMove1->addPointToPath(0,0);
		pMove1->addPointToPath(6.0*12-ROBOT_LENGTH-HALF_RAMP_WIDTH,0);
	
		pMove2->addPointToPath(0,0);
		pMove2->addPointToPath(76.91,58.38);
		pMove2->addPointToPath(-11.05,4.67);	// Angle towards target
	
		// No more moves; remove knProgrammedMove3 from the list.
		((StrategyShoot*)(m_lstStrategy[knShoot]))->getNextStates().pop_back();
	}
	
	reset();
	
    // TODO: set parameters on programmed moves properly
    
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
