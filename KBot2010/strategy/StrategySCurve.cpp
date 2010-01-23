#include "StrategySCurve.h"

#define S_CURVE_RUN_TIME  4.5
#define S_CURVE_NO_TRACK_TIME 4.0

/*
Constructor initalizes object
*/
StrategySCurve::StrategySCurve(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_nCallCount = 0;
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.02,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	m_gyroDriveCtrl->setDesiredValue(0.0);
	m_gyroDriveCtrl->setConstants(0.02, 0.0, 0.0);
    //m_pickup = m_kbot->getPickup();
	if (m_kbot->getAutoDirection()==AUTO_LEFT)
	{
		m_curveDir=45.0f;
	}
	else
	{
		m_curveDir=-45.0f;
	}
}

/*
Destructor cleans up
*/
StrategySCurve::~StrategySCurve()
{
    // Probably does not do anything in particular
}

/*
Keep running until time is up (may want to use some
distance measure instead)
*/
eState StrategySCurve::apply()
{
    eState nNewState = knSCurve;    // assume we will keep running
    if (TargetInSight() && ((int)m_nCallCount/50.0>S_CURVE_NO_TRACK_TIME))
    {
    	// Target spotted, switch to Track state
    	nNewState = knTrack;
        m_nCallCount = 0;   // reset call count in case we ever re-enter
    }
    else if (getKeepMoving())
    {
        m_nCallCount++; // track the number of times we have been called
        if (m_nCallCount<2*50)		//straight for first 2s
        {
        	m_gyroDriveCtrl->setDesiredValue(0.0);
        }
        else if (m_nCallCount<3*50)  // Curve left next 1s
        {
        	m_gyroDriveCtrl->setDesiredValue(m_curveDir/50.0/1.0*(int)(m_nCallCount-2));
        }
        else if (m_nCallCount<3.5*50)	// Straight for a bit
        {
        	m_gyroDriveCtrl->setDesiredValue(0.0);        	
        }
        else if (m_nCallCount<4.5*50)	// Curve right next 1s
        {
        	m_gyroDriveCtrl->setDesiredValue(-m_curveDir/50.0/1.0*(int)(m_nCallCount-3));
        }
        else						//straight for the rest
        {
        	m_gyroDriveCtrl->setDesiredValue(0.0);
        }
    	float xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
//    	m_robotDrive->ArcadeDrive(-0.5-((int)m_nCallCount)/50.0/2.0/5.0, xval, false);//take 5s to get up to full speed
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

void StrategySCurve::init()
{
	if (m_kbot->getAutoDirection()==AUTO_LEFT)
	{
	    printf("SCurve left\n");
		m_curveDir=45.0f;
	}
	else
	{
	    printf("SCurve right\n");
		m_curveDir=-45.0f;
	}
	m_kbot->getDriverStation()->SetDigitalOut(DS_START_STATE,true);
}

/*
Checks time to see if we need to keep moving.  Rather than muck
about with clocks etc I have just used the number of calls divided
by 20 Hz as an estimator of the time this strategy has been in
place.

@return true if we should keep moving, false otherwise
*/
bool StrategySCurve::getKeepMoving()
{
    bool bKeepMoving = true;
    if ((int)m_nCallCount/50.0 > S_CURVE_RUN_TIME)
    {
        bKeepMoving = false;
    }
    
    return bKeepMoving;
}

/* Check camera to see if we can see any targets */
bool StrategySCurve::TargetInSight()
{
	return false;
}
