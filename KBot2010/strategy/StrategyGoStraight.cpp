#include "StrategyGoStraight.h"

#define GO_STRAIGHT_RUN_TIME  5.0
#define GO_STRAIGHT_NO_TRACK_TIME 3.0

/*
Constructor initalizes object
*/
StrategyGoStraight::StrategyGoStraight(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.01,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	m_gyroDriveCtrl->setDesiredValue(0.0);
	m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
    //m_pickup = m_kbot->getPickup();
}

/*
Destructor cleans up
*/
StrategyGoStraight::~StrategyGoStraight()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyGoStraight::apply()
{
    eState nNewState = knGoStraight;    // assume we will keep running
    if (TargetInSight() && ((int)m_nCallCount/50.0>GO_STRAIGHT_NO_TRACK_TIME))
    {
    	// Target spotted, switch to Track state
    	nNewState = knTrack;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }
    else if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
    	float xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
    	m_robotDrive->ArcadeDrive(-1.0, xval, false);
    	//m_pickup->Drive(-0.4,true);
    }
    else
    {
        // time is up, switch to Spin state
        nNewState = knSpin;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }

    return nNewState;
}

void StrategyGoStraight::init()
{
    printf("GoStraight state\n");
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 20 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategyGoStraight::getKeepMoving()
{
    bool bKeepMoving = true;
    if ((int)m_nCallCount/50.0 > GO_STRAIGHT_RUN_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

/* Check camera to see if we can see any targets */
bool StrategyGoStraight::TargetInSight()
{
	return false;
}
