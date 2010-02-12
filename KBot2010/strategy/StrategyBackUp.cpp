#include "StrategyBackUp.h"

#define BACK_UP_TIME 2.5
/*
Constructor initalizes object

TODO:  handle new camera/target/dribbler hardware
*/
StrategyBackUp::StrategyBackUp(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
}

/*
Destructor cleans up
*/
StrategyBackUp::~StrategyBackUp()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyBackUp::apply()
{
    eState nNewState = knBackUp;    // assume we will keep running
    if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
        m_robotDrive->ArcadeDrive(1.0, 0.0, false);
    }
    else
    {
        // time is up, switch to Circle state
        nNewState = knCircle;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }

    return nNewState;
}

void StrategyBackUp::init()
{
    printf("BackUp state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_END_STATE,true);
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 200 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategyBackUp::getKeepMoving()
{
    bool bKeepMoving = true;
    if (m_nCallCount/20.0 > BACK_UP_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

