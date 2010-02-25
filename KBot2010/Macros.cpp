#include "Macros.h"
#include "Strategy.h"

RobotMacros::RobotMacros(KBot *kbot)
{
	m_kbot = kbot;
	m_macroState = mcDRIVE;
	m_macroCycle = 0;
	
	m_robotDrive = m_kbot->getKBotDrive();
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
		//TODO: why is this here? m_kbot->getWinchMotor()->Set(0.0f);
	}
	
	if (m_macroState == mcCAPTURE) {
		printf("Macros: CAPTURE\n");
		m_kbot->getManager()->getCaptureStrategy()->apply();
	}
	else if (m_macroState == mcAIM) {
		printf("Macros: AIM\n");
		m_kbot->getManager()->getAimStrategy()->apply();
	}
	else {	// default to driver control
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
	if (xval < 0)
	{
		xval *= -xval;	// preserve sign
	}
	else
	{
		xval *= xval;	// square for better control
	}
	if (abs(xval) > 0.05 || abs(yval)<0.05)
	{
		m_gyro->Reset();
	} else {
		m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
		xval += m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
	}
	yval = m_rightStick->GetY();
	//printf("x=%f  y=%f\n",xval,yval);
	m_robotDrive->ArcadeDrive(-yval, -xval, true);			// drive with arcade style (use right stick
}

// Allow complete operator control.
void RobotMacros::OperatorControl()
{
	if (true) //m_rightStick->GetTrigger()==0)
	{
		float zval = m_rightStick->GetZ();
		m_rollerMotor->Set(zval);
	}
}

