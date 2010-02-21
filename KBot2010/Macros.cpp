#include "Macros.h"
#include "Strategy.h"

static float kfWinchSpeed = 0.9f;

RobotMacros::RobotMacros(KBot *kbot)
{
	m_kbot = kbot;
	m_macroState = mcDRIVE;
	m_macroCycle = 0;
	
	m_robotDrive = m_kbot->getRobotDrive();
	m_winchMotor = m_kbot->getWinchMotor();
	m_rollerMotor = m_kbot->getRollerMotor();
	m_leftStick = m_kbot->getLeftStick();
	m_rightStick = m_kbot->getRightStick();
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.01,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	
	printf("constructed RobotMacros\n");
}


// Setting the state of macro that is being run. This action is happening in KBot.cpp
void RobotMacros::Set(MacroNames mac)
{
	m_macroState = mac;
}

// This method gets called every clock cycle (200Hz) from KBot.cpp
void RobotMacros::OnClock()
{
	m_macroCycle ++;
	
	if (m_macroState != mcWINCH) 
	{
		m_kbot->getWinchMotor()->Set(0.0f);
	}
	
	if (m_macroState == mcDEPLOY_ARM) {
		// do arm stuff
		float fY = m_leftStick->GetY();		
		if (fY > 0.5)
		{
			m_kbot->getArmRelease()->Set(false);
			m_kbot->getArmRetract()->Set(true);
		}
		else if (fY < -0.5)
		{
			m_kbot->getArmRelease()->Set(false);
			m_kbot->getArmRetract()->Set(true);
		}
		else
		{
			m_kbot->getArmRelease()->Set(true);
			m_kbot->getArmRetract()->Set(true);
		}
		DriverControl();
	}
	else if (m_macroState == mcWINCH) {
		m_kbot->getWinchMotor()->Set(kfWinchSpeed);		
	}
	else if (m_macroState == mcCAPTURE) {
		m_kbot->getManager()->getCaptureStrategy()->apply();
	}
	else if (m_macroState == mcAIM) {
		m_kbot->getManager()->getAimStrategy()->apply();
	}
	else if (m_macroState == mcSHOOT) {
		m_kbot->getKicker()->Kick();
	}
	else if (m_macroState == mcDRIVE) {
			DriverControl();
	}
	OperatorControl();
}

// Allow complete driver control.
void RobotMacros::DriverControl()
{
	float xval=0, yval=0;
	m_gyroDriveCtrl->setDesiredValue(0.0);
	xval = m_rightStick->GetX();
	if (abs(xval) > 0.05)
	{
		m_gyro->Reset();
	} else {
		m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
		xval += m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
	}
	yval = m_rightStick->GetY();
	//printf("x=%f  y=%f\n",xval,yval);
	m_robotDrive->ArcadeDrive(-yval, -xval, m_rightStick->GetTrigger());			// drive with arcade style (use right stick
}

// Allow complete operator control.
void RobotMacros::OperatorControl()
{
	float zval = m_rightStick->GetZ();
	m_rollerMotor->Set(zval);
}

