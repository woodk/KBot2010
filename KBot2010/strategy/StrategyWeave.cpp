#include "StrategyWeave.h"

/*!
Constructor initalizes object
*/
StrategyWeave::StrategyWeave(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_fRunTimeInSeconds = 0.5f; // five seconds to run to center
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getKBotDrive();
    //m_robotDrive->SetModifierConstant(AUTO_WEAVE_PERCENT);
    
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.05,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
}

/*!
Destructor cleans up
*/
StrategyWeave::~StrategyWeave()
{
    // Probably does not do anything in particular
}

/*!
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyWeave::apply()
{
    eState nNewState = knWeave;    // assume we will keep running
    if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
    	//float leftMod = 1.0 - abs(1.0 - (m_nCallCount % 150)/75.0);
    	//float rightMod = 1.0 - abs(1.0 - ((m_nCallCount+75) % 150)/75.0);
    	//m_robotDrive->ArcadeDrive(-1.0, 0.0,-leftMod,-rightMod, false);
    	m_gyroDriveCtrl->setDesiredValue(30*(abs(1.0 - (m_nCallCount % 150)/75.0)-0.5));
   		m_robotDrive->ArcadeDrive(-1.0, m_gyroDriveCtrl->calcPID(m_gyro->GetAngle()), false);
    }
    else
    {
        // time is up, switch into random state
        nNewState = knSpin;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }

    return nNewState;
}

void StrategyWeave::init()
{
    printf("Weave state\n");
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 200 Hz as an estimator of the time this strategy has been in
place.

\return true if we should keep moving, false otherwise
*/
bool StrategyWeave::getKeepMoving()
{
    bool bKeepMoving = true;
    if ((1.0f*m_nCallCount)/200 > m_fRunTimeInSeconds)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}
