#include "StrategyCapture.h"

#include "KBotDrive.h"

#include "DriverStationLCD.h"


bool bFirstCall = false;

/*
Constructor initalizes object

*/
StrategyCapture::StrategyCapture(KBot* kbot) : Strategy(kbot)
{
	m_kbot = kbot;
    // Create initial state here
	m_nLostCounter = 0;
	m_nNearCounter = 0;
	m_bFarLast = true;
	m_nGateCounter = 0;
	
	kfDriveForward = 0.15;	// % voltage to drive forward
	kfFarTurn = 0.2;		// % voltage for turn when far away
	knLostSweep = 1000;		// half second sweep
	knNearMax = 200;		// one second to get close after losing near sensor

	CAPTURE_MAX = 1000;
	
	m_nTime1 = -1;

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
    
    //printf("Gate Counter: %d\n",m_nGateCounter);
    //printf ("Gate Counter: %d/%d\n",m_nGateCounter, CAPTURE_MAX);
    // Keep turning until we find a target
    if (BallCaptured())
    {
    	if (bFirstCall)
    	{
    		printf("Gate: %d\n",m_nGateCounter);
	    	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line2, 1, "First Call Gate Counter: %d\n",m_nGateCounter);
	    	DriverStationLCD::GetInstance()->UpdateLCD();
    		bFirstCall = false;
    		m_nGateCounter = 0;
    		m_nTime1 = m_kbot->getKbotTime();
    	}
    	else
    	{
//    		if (m_kbot->getKbotTime() > m_nTime1)
	    	m_nGateCounter++;
	    	if (m_nGateCounter > CAPTURE_MAX)
	    	{
		    	printf("Got the ball %d times!!!!  Going to shoot state!\n",m_nGateCounter);
		    	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "Times: %d\n",m_nGateCounter);
		    	DriverStationLCD::GetInstance()->UpdateLCD();
		        // start tracking the target
		        nNewState = knShoot;
		        m_robotDrive->ArcadeDrive(0.0, 0.0, false);        
				m_nLostCounter = 0;
				m_nNearCounter = 0;
				m_nGateCounter = 0;
				m_nTime1 = -1;
	    	}
			else 
			{
				m_nLostCounter = 0;
				m_nNearCounter = 0;
	    	}
    	}
    }
    else	// move toward the ball and steer to capture
    {
    	if (m_kbot->getNearUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, 0.0, false);
    		m_nLostCounter = 0;
    		m_nNearCounter = 0;
    		m_bFarLast = false;
    	}
    	else if (m_kbot->getFarUltrasoundState())
    	{
    		m_robotDrive->ArcadeDrive(kfDriveForward, 0.0, false);
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
    		m_robotDrive->ArcadeDrive(kfDriveForward, 0.0, false);
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
	bFirstCall = true;
}

/* Check IR sensors to see if we have a ball */
bool StrategyCapture::BallCaptured()
{
	return m_kbot->getGateSensorState();
}
