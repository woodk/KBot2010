#include "StrategyAim.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyAim::StrategyAim(KBot* kbot) : Strategy(kbot)
{
	m_nNextState = knShoot;
	m_nState = knAim;
	
	m_fDistance = 0.0f;
	m_fLength = 0.0f;
	
	m_fAngle = 0.0f;
	m_fDirection = 0.0f;
	m_fAngleTolerance = 0.1f;
	
	m_fLastTime=0.0f;
	
	m_pidAngle = 0;
	m_pidDistance = 0;
}

/*
Destructor cleans up
*/
StrategyAim::~StrategyAim()
{
    // Probably does not do anything in particular
	delete m_pidAngle;
	delete m_pidDistance;
}

/*
Spin on the spot
*/
eState StrategyAim::apply()
{
    eState nNewState = knAim;    // assume we will keep running

    // update times
    float fDeltaT = 0.020f;

    // update position along path
    // TODO:  set calibrations properly, both linear and angular
    float fAverageSpeedFactor = 1.0;
    float fAngularSpeedFactor = 0.1;
    float fAngleFactor = 2.0;
    
	float fAverageSpeed = fAverageSpeedFactor*0.5f*(m_kbot->getLeftEncoder()->GetRate()+
								m_kbot->getRightEncoder()->GetRate());
	m_fDistance += fDeltaT*fAverageSpeed;
	float fTurnSpeed = fAngularSpeedFactor*(m_kbot->getRightEncoder()->GetRate()-
							m_kbot->getLeftEncoder()->GetRate());
	m_fAngle += fDeltaT*fTurnSpeed;

	vector<Target> vecTargets = m_kbot->getCamera()->findTargets();
	if (0 < vecTargets.size())
	{
		// do something to reject bad frames: eg:
		// keep list of last x values & time stamps
		// find most common within certain range
		//	 - make new list, copy of original
		//   - sort list by x
		//   - find median
		// remake this list from original, rejecting outliers (ie too far from median) 
		// pick lastest value in new list
		m_fDirection = fAngleFactor*vecTargets[0].m_xPos;
	}

	printf("%f %f %f\n",m_fDirection, m_fAngle, fTurnSpeed);

	if ((fabs(m_fDirection-m_fAngle) < m_fAngleTolerance))
	{
		nNewState = m_nNextState;  
	}
	else
	{
		// turning logic--may want PID here
		if (m_fDirection < m_fAngle)
		{
			m_fAngularSpeed =  0.1f;
		}
		else
		{
			m_fAngularSpeed = -0.1f;
		}    		
	}

	m_robotDrive->ArcadeDrive(m_fForwardSpeed, m_fAngularSpeed, false);

    return nNewState;
}

void StrategyAim::init()
{
    printf("Aim state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_SPIN_STATE,true);
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
	m_fForwardSpeed = 0.0f;
	m_fAngularSpeed = 0.0f;
}

/* Check camera to see if we can see any targets */
bool StrategyAim::TargetLocked()
{
	// TODO: set up to operate with new camera
	return false;
}
