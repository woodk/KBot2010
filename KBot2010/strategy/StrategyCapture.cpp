#include "StrategyCapture.h"

static const float kfDriveForward = 0.5;	// % voltage to drive forward
static const float kfFarTurn = 0.5;		// % voltage for turn when far away
static const float kfNearTurn = 0.25;		// % voltage for turn when nearer
static const int knLostSweep = 25;		// half second sweep
/*
Constructor initalizes object

*/
StrategyCapture::StrategyCapture(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_nLostCounter = 0;

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
    	// here we assume we have a ball in sight of something
    	// either one of the IR sensors or the near or far US
    	// sensors.  When the ball is in sight of BOTH IR
    	// sensors we are done, so that will never be true here.
    	if (m_kbot->getLeftIRSensorState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, kfNearTurn, false);
    		m_nLostCounter = 0;
    	}
    	else if (m_kbot->getRightIRSensorState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, -kfNearTurn, false);
    		m_nLostCounter = 0;
    	}
    	else if (m_kbot->getNearUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    	}
    	else if (m_kbot->getFarUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    	}
    	else	// we lost the ball!
    	{
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
    }

    return nNewState;
}

void StrategyCapture::init()
{
    printf("Capture state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
	m_nLostCounter = 0;
}

/* Check IR sensors to see if we have a ball */
bool StrategyCapture::BallCaptured()
{
	return (m_kbot->getLeftIRSensorState() && m_kbot->getRightIRSensorState());
}
