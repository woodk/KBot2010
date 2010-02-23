#include "StrategyProgrammedMove.h"

#include "KbotPID.h"

static const float kfAngularSpeed = 0.3f;
static const float kfForwardSpeed = 0.8f;

/*
Constructor initalizes object

*/
StrategyProgrammedMove::StrategyProgrammedMove(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_nNextState = knCapture;
	m_nState = knProgrammedMove1;
	m_fX = 0.0;
	m_fY = 0.0;
	m_nPathIndex = 0;
	
	m_fDistance = 0.0f;
	m_fLength = 0.0f;
	
	m_fAngle = 0.0f;
	m_fDirection = 0.0f;
	m_fAngleTolerance = 0.1f;
	
	m_fLastTime=0.0f;
	
	m_pidAngle = 0;
	m_pidDistance = 0;
	
	m_fStartAngle = 0.0f;
	m_fStartPosition = 0.0f;
}

/*
Destructor cleans up
*/
StrategyProgrammedMove::~StrategyProgrammedMove()
{
    // delete PID controllers
	delete m_pidAngle;
	delete m_pidDistance;
}

/*
Spin on the spot
*/
eState StrategyProgrammedMove::apply()
{
    eState nNewState = m_nState;    // assume we will keep running

    /*
     * The logic here is:  if distance is zero (we are at
     * the start of a segment) then turn until the angle 
     * is correct.  If the angle is correct and distance
     * is zero then move forward, keeping track of distance.
     * when distance is equal to or greater than length,
     * see if we are at the endpont of the path.  If we
     * are, then stop motors and return next state.  If
     * we are not, then compute new direction and return
     * current state.  Otherwise return current state
     */

    // update position along path
    // TODO:  check calibrations, both linear and angular
    float fPositionCalibration = 19.25/1440;  // inches per pulse
    float fAngleCalibration = fPositionCalibration/22.0f; // robot wheels are 22 inches apart
	float fAveragePosition = fPositionCalibration*(m_kbot->getLeftEncoder()->Get()+
								m_kbot->getRightEncoder()->Get());
	m_fDistance = fAveragePosition-m_fStartPosition;
	float fTurnAngle = fAngleCalibration*(m_kbot->getRightEncoder()->GetRate()-
							m_kbot->getLeftEncoder()->GetRate());
	m_fAngle = fTurnAngle-m_fStartAngle;
	
//	printf("%d %f %f %f %f %f %f\n",m_nPathIndex, m_fLength, fAverageSpeed, m_fDistance, m_fDirection, m_fAngle, fTurnSpeed);

	if ((fabs(m_fDirection-m_fAngle) < m_fAngleTolerance))
	{
		m_bTurning = false; // if we were turning we are no longer
		
		// we are headed in the right direction
		m_fForwardSpeed = -kfForwardSpeed;
		m_fAngularSpeed = 0.0f;
   		
		if (m_fDistance > m_fLength)
		{
			m_fForwardSpeed = 0.0f;
			// we are at end of segment
			if (m_nPathIndex+1 == (int)(m_vecX.size()-1))
			{
				// we are at end of path
				nNewState = m_nNextState;
			}
			else
			{
				// there is another segment to come
				m_fDistance = 0.0f;
				++m_nPathIndex;
				float fDeltaX = m_vecX[m_nPathIndex+1]-m_vecX[m_nPathIndex];
				float fDeltaY = m_vecY[m_nPathIndex+1]-m_vecY[m_nPathIndex];
				m_fLength = sqrt(fDeltaX*fDeltaX+fDeltaY*fDeltaY);
	    		m_fDirection = atan2(fDeltaY, fDeltaX);
	    		m_bTurning = true;
	    		m_fStartPosition = fPositionCalibration*(m_kbot->getLeftEncoder()->Get()+
	    									m_kbot->getRightEncoder()->Get());
	    		m_fDistance = fAveragePosition-m_fStartPosition;
	    		m_fStartAngle = fAngleCalibration*(m_kbot->getRightEncoder()->GetRate()-
	    								m_kbot->getLeftEncoder()->GetRate());
			}
		}
	}
	else
	{
		// turning logic
		if (m_fDirection < m_fAngle)
		{
			m_fAngularSpeed =  kfAngularSpeed;
		}
		else
		{
			m_fAngularSpeed = -kfAngularSpeed;
		}    		
	}

	// absolutely no forward motion allowed while turning
	// between segments
	if (m_bTurning)
	{
		m_fForwardSpeed = 0.0f;
		m_fAngularSpeed *= 2.0f;
		if (m_fAngularSpeed > 1.0f)
		{
			m_fAngularSpeed = 1.0f;
		}
	}
	m_robotDrive->ArcadeDrive(m_fForwardSpeed, m_fAngularSpeed, false);

    return nNewState;
}

void StrategyProgrammedMove::init()
{
    printf("Programmed Move state\n");
	float fDeltaX = m_vecX[m_nPathIndex+1]-m_vecX[m_nPathIndex];
	float fDeltaY = m_vecY[m_nPathIndex+1]-m_vecY[m_nPathIndex];
	m_fLength = sqrt(fDeltaX*fDeltaX+fDeltaY*fDeltaY);
	m_fDirection = atan2(fDeltaY, fDeltaX);
	m_fForwardSpeed = 0.0f;
	m_fAngularSpeed = 0.0f;
	m_fStartAngle = 0.0f;
	m_fStartPosition = 0.0f;
	m_bTurning = true;
}

/* Check camera to see if we can see any targets */
bool StrategyProgrammedMove::MoveComplete()
{
	// TODO: set up to operate with new camera
	return false;
}
