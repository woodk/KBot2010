#include "StrategyCurveLeft.h"

#include "Gyro.h"
#include "KbotPID.h"

#define CURVE_LEFT_RUN_TIME  3.0
#define CURVE_LEFT_NO_TRACK_TIME 2.5

/*
Constructor initalizes object
*/
StrategyCurveLeft::StrategyCurveLeft(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getKBotDrive();

	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.02,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	m_gyroDriveCtrl->setDesiredValue(0.0);
	m_gyroDriveCtrl->setConstants(0.02, 0.0, 0.0);
    //m_pickup = m_kbot->getPickup();
}

/*
Destructor cleans up
*/
StrategyCurveLeft::~StrategyCurveLeft()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategyCurveLeft::apply()
{
    eState nNewState = knCurveLeft;    // assume we will keep running
    if (TargetInSight()&&((int)m_nCallCount/50.0>CURVE_LEFT_NO_TRACK_TIME))
    {
    	// Target spotted, switch to Track state
    	nNewState = knTrack;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }
    else if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
    	m_gyroDriveCtrl->setDesiredValue(45.0f/50.0/3.0*(int)m_nCallCount);//take 5s to curve 45deg
    	float xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
    	m_robotDrive->ArcadeDrive(-0.5-((int)m_nCallCount)/50.0/2.0/5.0, xval, false);//take 5s to get up to full speed
        //m_robotDrive->ArcadeDrive(-1.0, -0.9, false);
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

void StrategyCurveLeft::init()
{
    printf("CurveLeft state\n");
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 200 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategyCurveLeft::getKeepMoving()
{
    bool bKeepMoving = true;
    if ((int)m_nCallCount/50.0 > CURVE_LEFT_RUN_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

/* Check camera to see if we can see any targets */
bool StrategyCurveLeft::TargetInSight()
{
	return false;  //m_camera->lockedOn();
}
