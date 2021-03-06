#include "RobotManager.h"

#include "Strategy.h"
#include "StrategyAim.h"
#include "StrategyCapture.h"
#include "StrategyCapture2.h"
#include "StrategyProgrammedMove.h"
#include "StrategySearch.h"
#include "StrategyShoot.h"
#include "StrategyCircle.h"
#include "StrategyBackUp.h"
#include "StrategyInactive.h"

/*!
Constructor sets up initial strategy
*/
RobotManager::RobotManager(KBot *kbot)
{
	m_kbot = kbot;
	m_driverStation = m_kbot->getDriverStation();
	
	m_nStartState = knInitial; // Initial state is set in the manager state (ManagerForward, ManagerMidfield, ManagerDefense)

    for (int nIndex = 0; nIndex < knInitial; nIndex++)
    {
        m_lstStrategy[nIndex] = 0;
    }
    
    // create common stratgies
    m_lstStrategy[knAim] = new StrategyAim(m_kbot);
    m_lstStrategy[knBackUp] = new StrategyBackUp(m_kbot);   
    m_lstStrategy[knCircle] = new StrategyCircle(m_kbot);
    m_lstStrategy[knCapture] = new StrategyCapture(m_kbot);
    m_lstStrategy[knCapture2] = new StrategyCapture2(m_kbot);
    m_lstStrategy[knProgrammedMove1] = new StrategyProgrammedMove(m_kbot);
    m_lstStrategy[knProgrammedMove2] = new StrategyProgrammedMove(m_kbot);
    m_lstStrategy[knProgrammedMove3] = new StrategyProgrammedMove(m_kbot);
    m_lstStrategy[knSearch] = new StrategySearch(m_kbot);
    m_lstStrategy[knShoot] = new StrategyShoot(m_kbot);
    m_lstStrategy[knInactive] = new StrategyInactive(m_kbot);

    reset();

}

/*!
Destructor cleans up strategy list
*/
RobotManager::~RobotManager()
{
    for (int nIndex = 0; nIndex < knInitial; nIndex++)
    {
        delete m_lstStrategy[nIndex];
    }
}

/*!
If the manager is active then passes control on to the
current strategy, and changes strategies for the next
step if requested by the current strategy.
*/
void RobotManager::onClock(bool bActive)
{
    if (bActive)
    {
        eState nCurrentState = getState();  // get the current state and apply strategy
        if (knInitial == getState())
        {
    		setState(m_nStartState);
        	m_lstStrategy[nCurrentState]->init();
        }

        eState nNewState = m_lstStrategy[nCurrentState]->execute();
        setState(nNewState);
        if (nNewState != nCurrentState)
            m_lstStrategy[nNewState]->init();
    }
}

// Sets up intial state data for first strategy
void RobotManager::init() 
{
	m_lstStrategy[m_nStartState]->init();
}

void RobotManager::reset()
{
    setState(m_nStartState);
}
