#include "Macros.h"
#include "Strategy.h"

#include "Gyro.h"
#include "Joystick.h"
#include "KBot.h"
#include "KBotDrive.h"
#include "KBotPID.h"
#include "RobotManager.h"
#include "SpeedController.h"

#define TURN_BOOST_VALUE 0.15

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
	
	/*if (m_macroState != mcWINCH) 
	{
		m_kbot->getWinchMotor()->Set(0.0f);
	}*/
	
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
	if (fabs(xval) > 0.05 || fabs(yval)<0.05)
	{
		m_gyro->Reset();
	} else {
		m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
		xval += m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
	}
	yval = m_rightStick->GetY();
	if (m_rightStick->GetRawButton(LEFT_BOOST_BUTTON))
	{
		xval -= TURN_BOOST_VALUE;
		printf("left boost before=%f after=%f\n",xval-TURN_BOOST_VALUE,xval);
	}
	if (m_rightStick->GetRawButton(RIGHT_BOOST_BUTTON))
	{
		xval += TURN_BOOST_VALUE;
		printf("right boost before=%f after=%f\n",xval+TURN_BOOST_VALUE,xval);
	}
	if (m_rightStick->GetRawButton(TURN_BOOST_BUTTON))
	{
		printf("left boost before=%f",xval);
		if (xval>0)
			xval += TURN_BOOST_VALUE;
		else
			xval -= TURN_BOOST_VALUE;
		printf(" after=%f\n",xval);		
	}
	//printf("x=%f  y=%f\n",xval,yval);
	m_robotDrive->ArcadeDrive(-yval, -xval, false);			// drive with arcade style (use right stick) no squared inputs because we already squared x
}
int counter=0;

// Allow complete operator control.
void RobotMacros::OperatorControl()
{
	if ((++counter % 10) == 0) //m_rightStick->GetTrigger()==0)
	{
		float zval;
		if (m_kbot->getLeftStick()->GetRawButton(ROLLER_STOP_BUTTON))
			zval=0;
		else
			zval = m_rightStick->GetZ();
		m_rollerMotor->Set(zval);
		m_kbot->getWinchMotor()->Set(0.0f);
	}
}

