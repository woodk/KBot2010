#include "robotManager.h"

#include "Strategy.h"
#include "StrategyGoStraight.h"
#include "StrategySCurve.h"
#include "StrategyCurveLeft.h"
#include "StrategyCurveRight.h"
#include "StrategyTurn90.h"
#include "StrategyTurn45.h"
#include "StrategyGoAlongEnd.h"
#include "StrategyGoAlongEdge.h"
#include "StrategyWeave.h"
#include "StrategySpin.h"
#include "StrategyCircle.h"
#include "StrategyTrack.h"
#include "StrategyBackUp.h"
#include "StrategyInactive.h"

/*!
Constructor sets up initial strategy
*/
RobotManager::RobotManager(KBot *kbot)
{
	m_kbot = kbot;
	m_driverStation = m_kbot->getDriverStation();
	
    setState(knInitial);

    // create simple list of stratgies
    m_lstStrategy[knGoStraight] = new StrategyGoStraight(m_kbot);
    m_lstStrategy[knSCurve] = new StrategySCurve(m_kbot);
    
    m_lstStrategy[knCurveLeft] = new StrategyCurveLeft(m_kbot);
    m_lstStrategy[knCurveRight] = new StrategyCurveRight(m_kbot);
    m_lstStrategy[knTurn90] = new StrategyTurn90(m_kbot);
    m_lstStrategy[knTurn45] = new StrategyTurn45(m_kbot);
    m_lstStrategy[knGoAlongEnd] = new StrategyGoAlongEnd(m_kbot);
    m_lstStrategy[knGoAlongEdge] = new StrategyGoAlongEdge(m_kbot);
    
    m_lstStrategy[knWeave] = new StrategyWeave(m_kbot);
    m_lstStrategy[knSpin] = new StrategySpin(m_kbot);
    m_lstStrategy[knCircle] = new StrategyCircle(m_kbot);
    m_lstStrategy[knTrack] = new StrategyTrack(m_kbot);
    m_lstStrategy[knBackUp] = new StrategyBackUp(m_kbot);
    m_lstStrategy[knInactive] = new StrategyInactive(m_kbot);
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
        eState nCurrentState=knSCurve; //knGoStraight;
        if (knInitial == getState())
        {
    		if (m_kbot->getAutoMode()==AUTO_STRAIGHT)
    			nCurrentState=knSCurve; //knGoStraight;
    		else if (m_kbot->getAutoMode()==AUTO_CURVE)
    		{
    			if (m_kbot->getAutoDirection()==AUTO_LEFT)
    				nCurrentState=knCurveLeft;
    			else
    				nCurrentState=knCurveRight;
    		}
    		else if (m_kbot->getAutoMode()==AUTO_45_LOAD)
    		{
    			// Sequence is Turn45, GoAlongEnd, Turn90, inactive
    			((StrategyTurn45 *)m_lstStrategy[knTurn45])->setNextState(knGoAlongEnd);
   				nCurrentState=knTurn45;
    		}
    		else if (m_kbot->getAutoMode()==AUTO_90_LOAD)
    		{
    			// Sequence is Turn90, GoAlongEdge, Turn45, LoadBalls
    			((StrategyTurn90 *)m_lstStrategy[knTurn90])->setNextState(knGoAlongEdge);
   				nCurrentState=knTurn90;
    		}
    		
    		setState(nCurrentState);
        	m_lstStrategy[nCurrentState]->init();
        }

        nCurrentState = getState();  // get the current state and apply strategy
        eState nNewState = m_lstStrategy[nCurrentState]->apply();
        setState(nNewState);
        if (nNewState == knGoAlongEnd || nNewState == knGoAlongEdge)
        {
			((StrategyTurn45 *)m_lstStrategy[knTurn45])->setNextState(knLoadBalls);
        }
        if (nNewState != nCurrentState)
            m_lstStrategy[nNewState]->init();
    }
}
void RobotManager::reset()
{
    setState(knInitial);
}
