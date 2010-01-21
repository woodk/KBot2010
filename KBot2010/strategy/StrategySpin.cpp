#include "StrategySpin.h"

/*
Constructor initalizes object
*/
StrategySpin::StrategySpin(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
    m_shooter = m_kbot->getShooter();
}

/*
Destructor cleans up
*/
StrategySpin::~StrategySpin()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategySpin::apply()
{
    eState nNewState = knSpin;    // assume we will keep running

    // Keep turning until we find a target
    if (TargetInSight())
    {
        // start tracking the target
        nNewState = knTrack;
    }
    else
    {
    	if (m_kbot->getAutoDirection()==AUTO_LEFT)
    	{
    		m_robotDrive->ArcadeDrive(-0.8, -0.9, false);
    	}
    	else
    	{
    		m_robotDrive->ArcadeDrive(-0.8, 0.9, false);
    	}
		m_shooter->Drive(0.0, true);
    }

    return nNewState;
}

void StrategySpin::init()
{
    printf("Spin state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_SPIN_STATE,true);
    //m_robotDrive->setTorque(120);
}

/* Check camera to see if we can see any targets */
bool StrategySpin::TargetInSight()
{
	return m_camera->lockedOn();
}
