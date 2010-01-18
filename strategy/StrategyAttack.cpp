#include "StrategyAttack.h"

/*
Constructor initalizes object
*/
StrategyAttack::StrategyAttack(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
	m_cameraDriveCtrl = new KbotPID(0.1, 0.001, 0.0005);
	m_cameraDriveCtrl->resetErrorSum();
	m_cameraDriveCtrl->setDesiredValue(5.0); // Desired distance from target

	m_cameraTurnCtrl = new KbotPID(0.1, 0.001, 0.0005);
	m_cameraTurnCtrl->resetErrorSum();
	m_cameraTurnCtrl->setDesiredValue(0.0);

	m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
    m_camera = m_kbot->getCamera();
    m_shooter = m_kbot->getShooter();
    m_pickup = m_kbot->getPickup();
    m_attackCycles=0;
    m_lossCount=0;
}

/*
Destructor cleans up
*/
StrategyAttack::~StrategyAttack()
{
    // Probably does not do anything in particular
}

/*
Keep attacking while target is in view,
otherwise go to Attack mode to search for
new targets.
*/
eState StrategyAttack::apply()
{
    eState nNewState = knAttack;    // assume we will keep attacking
    
    if (getTargetInSight())
    {
   		
    	float realY=m_camera->getDistanceToTarget();
  		m_yval = m_cameraDriveCtrl->calcPID(realY);

  		float realX=m_camera->getTargetX()*realY;
    	m_xval = m_cameraTurnCtrl->calcPID(-realX);

    	// Could try this, from the tracker sample code:
    	//m_xval = m_camera->getCentreMassXNormalized();
    	//m_robotDrive->Drive(m_yval,m_xval);

    	// Or, using PID: (change PID Kp to 1.0)
    	//realx = m_camera->getCentreMassXNormalized();
    	//m_xval = m_cameraTurnCtrl->calcPID(realx);
    	//m_robotDrive->Drive(m_yval,m_xval);
    	
    	// Or could try this:
    	// m_xval = atan(m_camera->getCentreMassXNormalized())*4.0/3.1416
    	//m_robotDrive->Drive(m_yval,m_xval);
    	
   		//printf("ATTACK PID: in x=%5.2f y=%5.4f out  x=%5.2f, y=%5.2f\n",-m_camera->getTargetX(),m_camera->getTargetY(),m_xval,yval);
    	m_robotDrive->ArcadeDrive(m_yval, m_xval, false);
    	
		float shooterSpeed;
		if ( m_camera->lockedOn())
		{
			shooterSpeed = (m_camera->getTargetY()-CAM_MIN)*(SHOOT_MAX-SHOOT_MIN)/(CAM_MAX-CAM_MIN)+SHOOT_MIN;
			// KEVIN or base this on m_camera->getDistanceToTarget() which is in ft
			//printf("Auto shooter speed %5.2f; targetY= %5.2f\n",shooterSpeed,m_camera->getTargetY());
		}
		else // Lost camera lock; assume we are too close
		{
			shooterSpeed = SHOOT_MIN;
			//printf("Lost camera lock; speed %5.2f\n",shooterSpeed);
		}
		
		if (getTargetAttackable())
		{
			m_attackCycles++;
	    	if (m_attackCycles<25) // adjust shooter to correct speed
			{
				m_shooter->Drive(shooterSpeed,true);
				m_pickup->Drive(0.0,true);
			}
	    	else if (m_attackCycles<200) // spin and shoot
	    	{
				m_shooter->Drive(shooterSpeed,true);
				m_pickup->Drive(-1.0,true);
			}
			else // stop shooting
			{
				m_shooter->Drive(0.0, true);
				m_pickup->Drive(0.0,true);
		    	nNewState = knBackUp;
			}
		}
		/*else
		{
			m_attackCycles=0;
		}*/
    	m_lossCount=0;
    }
    else
    {
    	m_lossCount++;
    	m_xval *= 0.98;
    	m_yval *= 0.98;
    	m_robotDrive->Drive(m_yval,m_xval);
   		//printf("LOST ATTACK PID: in x=%5.2f y=%5.4f out  x=%5.2f\n",-m_camera->getTargetX(),m_camera->getTargetY(),m_xval);
    	if (m_lossCount>100)
    	{
	    	printf("Attack lost target\n");
	       // we have lost the target, switch to Spin strategy
	        nNewState = knSpin;
    	}
    }

    return nNewState;
}
/*
Checks camera to see if target is close enough to attack

@return true if close enough, false otherwise
*/
bool StrategyAttack::getTargetAttackable()
{
	//printf("particle size = %lf   camera servo = %5.2f\n",m_camera->getParticleSize(),m_camera->getTargetY());
    //return m_camera->getParticleSize()>2.0;
	//return m_camera->getParticleSize()>8.0 || m_camera->getTargetY()<0.17; // ATTACK!! was 0.23
	return m_camera->getDistanceToTarget()<8.0;
}

/*
 *Checks camera to see if target is still in view,
 *@return true if we can see target robot, false otherwise
 */
bool StrategyAttack::getTargetInSight()
{
    return m_camera->lockedOn();
}

void StrategyAttack::init()
{
    printf("Attack state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_ATTACK_STATE,true);
	m_camera->setPanEnabled(false);
	m_camera->setFixedServoPos(0.45);	// just a guess

	m_attackCycles=0;
    m_lossCount=0;
}
