#include "StrategyTrack.h"

/*
Constructor initalizes object
*/
StrategyTrack::StrategyTrack(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_cameraTurnCtrl = new KbotPID(0.1, 0.001, 0.0005);
	m_cameraTurnCtrl->resetErrorSum();
	m_cameraTurnCtrl->setDesiredValue(0.0);
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
    m_shooter = m_kbot->getShooter();
    m_lossCount=0;
}

/*
Destructor cleans up
*/
StrategyTrack::~StrategyTrack()
{
    // Probably does not do anything in particular
}

/*
Keep hunting while target is in view,
otherwise go to random mode to search for
new targets.  Go to attack strategy when
we get close.
*/
eState StrategyTrack::apply()
{
    eState nNewState = knTrack;    // assume we will keep hunting
    if (getTargetAttackable())
    {
        nNewState = knAttack;
    }
    else if (getTargetEscaped())
    {
    	m_lossCount++;
    	m_xval *= 0.98;
   		//printf("LOST targ X= %5.4f Y: %5.4f xval= %5.4f\n",-m_camera->getTargetX(),m_camera->getTargetY(), m_xval);
    	m_robotDrive->ArcadeDrive(-0.75, m_xval, false); //.75
    	//Does this want to be ->Drive(-0.75, m_xval);
    	
    	if (m_lossCount>100)
    	{
	        // we have lost the target, switch to spin strategy
			m_shooter->Drive(0.0, true); // give up on shooter
			printf("lost target, going inactive\n");
	        nNewState = knSpin; 
    	}
    }
    else    // still hunting
    {
    	m_lossCount=0;
    	float d=m_camera->getDistanceToTarget();
    	float realx=m_camera->getTargetX()*d;
    	m_xval = m_cameraTurnCtrl->calcPID(-realx);
   		//printf("d= %5.4f realx=%5.4f xval= %5.4f\n",d, realx, m_xval);
    
    	m_robotDrive->ArcadeDrive(-0.9, m_xval, false); //.75
    	
    	// Could try this, from the tracker sample code:
    	//m_xval = m_camera->getCentreMassXNormalized();
    	//m_robotDrive->Drive(-0.9,m_xval);

    	// Or, using PID: (change PID Kp to 1.0)
    	//realx = m_camera->getCentreMassXNormalized();
    	//m_xval = m_cameraTurnCtrl->calcPID(realx);
    	//m_robotDrive->Drive(-0.9,m_xval);
    	
    	
    	// Or could try this:
    	// m_xval = atan(m_camera->getCentreMassXNormalized())*4.0/3.1416
    	//m_robotDrive->Drive(-0.9,m_xval);
    	
    	
    	// Get the shooter up to full speed to be ready
		//m_shooter->Drive(1.0, true);
    }

    return nNewState;
}


/*
Checks camera to see if target is close enough to attack

@return true if close enough, false otherwise
*/
bool StrategyTrack::getTargetAttackable()
{
	//printf("particle size = %lf   camera servo = %5.2f\n",m_camera->getParticleSize(),m_camera->getTargetY());
    //return m_camera->getParticleSize()>2.0;
	//return m_camera->getParticleSize()>4.5 || m_camera->getTargetY()<0.25; // ATTACK!! was 0.23
	//printf("dist %5.4f\n",m_camera->getDistanceToTarget());
	return false;	//m_camera->getDistanceToTarget()<20.0;
}

/*
Checks camera to see if target has escaped

@return true if escaped, false otherwise
*/
bool StrategyTrack::getTargetEscaped()
{
    return !m_camera->lockedOn();
}

void StrategyTrack::init()
{
    printf("Track state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
	m_camera->setPanEnabled(false);
	m_camera->setFixedServoPos(0.5);
	
    m_lossCount=0;
    //m_robotDrive->setTorque(110);
}

