#include "StrategyCapture.h"

static const float kfDriveForward = 0.25;	// % voltage to drive forward
static const float kfFarTurn = 0.25;		// % voltage for turn when far away
static const int knLostSweep = 25;		// half second sweep
static const int knNearMax = 25;		// half second to get close after losing near sensor
/*
Constructor initalizes object

*/
StrategyCapture::StrategyCapture(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_nLostCounter = 0;
	m_nNearCounter = 0;
	m_bFarLast = true;

}

/*
Destructor cleans up
*/
StrategyCapture::~StrategyCapture()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyCapture::apply()
{
    eState nNewState = knCapture;    // assume we will keep running

    // Keep turning until we find a target
    if (BallCaptured())
    {
        // start tracking the target
        nNewState = knAim;
        m_robotDrive->ArcadeDrive(0.0, 0.0, false);        
    }
    else	// move toward the ball and steer to capture
    {
    	if (m_kbot->getNearUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(-kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    		m_nNearCounter = 0;
    		m_bFarLast = false;
    	}
    	else if (m_kbot->getFarUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(-kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    		m_nNearCounter = 0;
    		m_bFarLast = true;
    	}
    	else if (m_bFarLast)	// we lost the ball!
    	{
    		m_nNearCounter = 0;
    		++m_nLostCounter;
    		if (knLostSweep < m_nLostCounter)	// turn one way
    		{
    			m_robotDrive->ArcadeDrive(0.0, kfFarTurn, false);
    		}
    		else	// turn the other
    		{
    			m_robotDrive->ArcadeDrive(0.0, -kfFarTurn, false);
    		}
    		if (knLostSweep*3 < m_nLostCounter) // factor of 3 means we go:  <- -->
    		{
    			m_nLostCounter = 0;
    			nNewState = knSearch;	// could not find, so go back to search
    		}
    	}
    	else if (m_nNearCounter < knNearMax)	// we got too close for the ultrasound to see, keep going foward
    	{
    		m_robotDrive->ArcadeDrive(-kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    		++m_nNearCounter;
    		m_bFarLast = false;
    	}
    	else	// we lost the ball close-in
    	{
    		m_nNearCounter = 0;
    		m_bFarLast = true;	// this will force a sweep to redetect the ball
    	}
    }

    return nNewState;
}

void StrategyCapture::init()
{
    printf("Capture state\n");
	m_nLostCounter = 0;
}

/* Check IR sensors to see if we have a ball */
bool StrategyCapture::BallCaptured()
{
	return (0 == m_kbot->getGateSensorState());
}
