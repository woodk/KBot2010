#include "StrategyAim.h"

#include "Encoder.h"
#include "KbotCamera.h"
#include "KbotDrive.h"
#include "KbotPID.h"
#include "Target.h"

#include <algorithm>
#include <vector>

bool xSortPredicate(camPoint& point1, camPoint& point2)
{
	return point1.m_x < point2.m_x;
}

bool tSortPredicate(camPoint& point1, camPoint& point2)
{
	return point1.m_t < point2.m_t;
}

/*
Constructor initalizes object

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
	
	m_fForwardSpeed =0.0f;
	
	for (int i=0; i<20; i++)
	{
		m_vecPoints.push_back(camPoint(0.0,0.0));
	}
	m_pointCounter = 0;
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
 //   float fPositionCalibration = 19.25/1440;  // inches per pulse
 //   float fAngleCalibration = fPositionCalibration/22.0f; // robot wheels are 22 inches apart
    
    float fAverageSpeedFactor = 1.0;
    float fAngularSpeedFactor = 0.1;
    float fAngleFactor = 2.0;
    
	float fAverageSpeed = fAverageSpeedFactor*0.5f*(m_kbot->getLeftEncoder()->GetRate()+
								m_kbot->getRightEncoder()->GetRate());
	m_fDistance += fDeltaT*fAverageSpeed;
	float fTurnSpeed = -fAngularSpeedFactor*(m_kbot->getRightEncoder()->GetRate()-
							m_kbot->getLeftEncoder()->GetRate());
	m_fAngle += fDeltaT*fTurnSpeed;

	std::vector<Target> vecTargets = m_kbot->getCamera()->findTargets();
	if (vecTargets[0].m_score >= MINIMUM_SCORE)
	{
		// do something to reject bad frames: eg:
		// keep list of last x values & time stamps
		
		m_pointCounter++;
		m_vecPoints[m_pointCounter%20]=camPoint(vecTargets[0].m_xPos, m_pointCounter);
		if (m_pointCounter>=20)
		{
			if (m_pointCounter==20) {
				m_vecPoints[19].m_x += 0.5;
				for (int i=1; i<=20; i++)
					printf("%2.1f  ",m_vecPoints[i%20].m_x);
				printf("\n");
			}
			// find most common within certain range
			//	 - make new list, copy of original
			vector<camPoint> tempPoints = m_vecPoints;
			//   - sort list by x
			std::sort(tempPoints.begin(), tempPoints.end(), xSortPredicate);
			//   - find median
			float median = tempPoints[(m_pointCounter+10)%20].m_x;
			// remake this list from original, rejecting outliers (ie too far from median)
			float x1= m_vecPoints[m_pointCounter%20].m_x;
			float x2= m_vecPoints[(m_pointCounter-1+20)%20].m_x;
			float result;
			if (abs(x1 - median)>abs(x2 - median))
			{
				result = x2;
			} else {
				result = x1;
			}
			if (m_pointCounter==20) {
				printf("x1=%2.1f  x2=%2.1f  median=%2.1f  result=%2.1f\n",x1,x2,median,result);
			}
			// pick best of last 2 values
			m_fAngularSpeed = fAngleFactor*result;
		}
		else
			m_fAngularSpeed = fAngleFactor*vecTargets[0].m_xPos;
		
	}

	printf("%f %f %f\n",m_fDirection, m_fAngle, fTurnSpeed);

	if ((fabs(m_fDirection-m_fAngle) < m_fAngleTolerance))
	{
		nNewState = m_nNextState;  
		m_pointCounter = 0;	// so ring buffer refills next time
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
	m_fForwardSpeed = 0.0f;	// -0.05 TODO:  uncomment this
	m_fAngularSpeed = 0.0f;
}

/* Check camera to see if we can see any targets */
bool StrategyAim::TargetLocked()
{
	// TODO: set up to operate with new camera
	return false;
}
