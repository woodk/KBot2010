#include "StrategyLoadBalls.h"

#define LOAD_BALLS_TIME 20.0
/*
Constructor initalizes object
*/
StrategyLoadBalls::StrategyLoadBalls(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
	//m_gyro = m_kbot->getGyro();
	//m_gyroDriveCtrl = new KbotPID(0.01,0.0,0.0);
	//m_gyro->Reset();
	//m_gyroDriveCtrl->resetErrorSum();
	//m_gyroDriveCtrl->setDesiredValue(0.0);
	//m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
    m_pickup = m_kbot->getPickup();
}

/*
Destructor cleans up
*/
StrategyLoadBalls::~StrategyLoadBalls()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyLoadBalls::apply()
{
    eState nNewState = knLoadBalls;    // assume we will keep running
    /*if (TargetInSight())
    {
    	// Target spotted, switch to Track state
    	nNewState = knTrack;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }
    else*/ if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
    	//float xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
    	m_robotDrive->ArcadeDrive(0.0, 0.0, false);
    	m_pickup->Drive(-0.6,true);
    }
    else
    {
        // time is up, switch to Spin state
        nNewState = knSpin;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }

    return nNewState;
}

void StrategyLoadBalls::init()
{
    printf("LoadBalls state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_START_STATE,true);
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 200 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategyLoadBalls::getKeepMoving()
{
    bool bKeepMoving = true;
    if (m_nCallCount/50.0 > LOAD_BALLS_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

/* Check camera to see if we can see any targets */
/*bool StrategyLoadBalls::TargetInSight()
{
	return m_camera->lockedOn();
}*/
