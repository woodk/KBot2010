#include "StrategyTurn90.h"

#include "Gyro.h"
#include "KbotPID.h"

#define TURN_90_ABORT_TIME 2.0

/*
Constructor initalizes object
*/
StrategyTurn90::StrategyTurn90(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getKBotDrive();
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.015,0.0,0.0,0.2);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	m_gyroDriveCtrl->setConstants(0.015, 0.0, 0.0);
	if (m_kbot->getAutoDirection())
	{
		printf("90 left\n");
		m_gyroDriveCtrl->setDesiredValue(90.0f);
	}
	else
	{
		printf("90 right\n");
		m_gyroDriveCtrl->setDesiredValue(-90.0f);
	}
}

/*
Destructor cleans up
*/
StrategyTurn90::~StrategyTurn90()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyTurn90::apply()
{
    eState nNewState = knTurn90;    // assume we will keep running
    if (getKeepTurning())
    {
        m_nCallCount++; // track the number of times we have been called
    	float xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
    	m_robotDrive->ArcadeDrive(0.0, xval, false);
  }
    else
    {
    	// brake
    	m_robotDrive->ArcadeDrive(0.0, 0.0);
        // time is up, switch to next state
        nNewState = m_nextState;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }

    return nNewState;
}

void StrategyTurn90::init()
{
	m_gyro->Reset();
	if (m_kbot->getAutoDirection())
	{
		m_gyroDriveCtrl->setDesiredValue(90.0f);
	    printf("Turn90 Left state\n");
	}
	else
	{
		m_gyroDriveCtrl->setDesiredValue(-90.0f);
	    printf("Turn90 Right state\n");
	}
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 50 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategyTurn90::getKeepTurning()
{
    bool bKeepMoving = true;
    printf("angle: %f\n",m_gyro->GetAngle());
    //if (abs(m_gyro->GetAngle())>89.0 || (int)m_nCallCount/50.0 > TURN_90_ABORT_TIME)
    if (m_gyroDriveCtrl->isDone() || (int)m_nCallCount/50.0 > TURN_90_ABORT_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

void StrategyTurn90::setNextState(eState next)
{
	m_nextState = next;
}
