#include "Macros.h"

RobotMacros::RobotMacros(KBot *kbot)
{
	m_kbot = kbot;
	m_macroState = mcNONE;
	m_macroCycle = 0;
	lastTrigger = false;
	triggerCycle = 1000;		// Starting at a big number makes sure the shooter
								// doesn't reverse when the robot is first turned on
	
	m_robotDrive = m_kbot->getRobotDrive();
	m_leftStick = m_kbot->getLeftStick();
	m_rightStick = m_kbot->getRightStick();
	m_gyro = m_kbot->getGyro();
	m_gyroDriveCtrl = new KbotPID(0.01,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();

	// TODO:  get Kicker, Dribbler, Arm, etc.
	
	printf("constructed RobotMacros\n");
}


// Setting the state of macro that is being run. This action is happening in KBot.cpp
void RobotMacros::Set(MacroNames mac)
{
	m_macroState = (MacroNames)(m_macroState | mac);
}

// Setting the state of macro that is being run. This action is happening in KBot.cpp
void RobotMacros::Clear(MacroNames mac)
{
	m_macroState = (MacroNames)(m_macroState & !mac);
}

// This method gets called every clock cycle (200Hz) from KBot.cpp
void RobotMacros::OnClock()
{
	m_macroCycle ++;
	if (m_macroState & mcWEAVE) {
		WeaveDrive();
	} else {
		OperatorDrive();
	}
	if (m_macroState & mcTURNLEFT) {
		Turn(mcTURNLEFT);
	}
	if (m_macroState & mcTURNRIGHT) {
		Turn(mcTURNRIGHT);
	}
	OperatorControl();
}

// Allow complete operator drive.
void RobotMacros::OperatorDrive()
{
	float xval=0, yval=0;
	if (m_rightStick->GetRawButton(LEFT_90_BUTTON))
	{
		if (m_firstTurnCycle)
		{
			m_firstTurnCycle = false;
			m_gyro->Reset();
			m_gyroDriveCtrl->setDesiredValue(-90.0);
		}
		m_gyroDriveCtrl->setConstants(0.1, 0.0, 0.0);
		xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
		
	}
	else if (m_rightStick->GetRawButton(RIGHT_90_BUTTON))
	{
		if (m_firstTurnCycle)
		{
			m_firstTurnCycle = false;
			m_gyro->Reset();
			m_gyroDriveCtrl->setDesiredValue(90.0);
		}
		m_gyroDriveCtrl->setConstants(0.1, 0.0, 0.0);
		xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());		
	}
	else if (m_rightStick->GetRawButton(LEFT_180_BUTTON))
	{
		if (m_firstTurnCycle)
		{
			m_firstTurnCycle = false;
			m_gyro->Reset();
			m_gyroDriveCtrl->setDesiredValue(-180.0);
		}
		m_gyroDriveCtrl->setConstants(0.1, 0.0, 0.0);
		xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());		
	}
	else if (m_rightStick->GetRawButton(RIGHT_180_BUTTON))
	{
		if (m_firstTurnCycle)
		{
			m_firstTurnCycle = false;
			m_gyro->Reset();
			m_gyroDriveCtrl->setDesiredValue(180.0);
		}
		m_gyroDriveCtrl->setConstants(0.1, 0.0, 0.0);
		xval = m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());		
	}
	else
	{	
		m_gyroDriveCtrl->setDesiredValue(0.0);
		m_firstTurnCycle=true;
		xval = m_rightStick->GetX();
		if (abs(xval) > 0.05)
		{
			m_gyro->Reset();
		} else {
			m_gyroDriveCtrl->setConstants(0.01, 0.0, 0.0);
			xval += m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
		}
		yval = m_rightStick->GetY();
	}
	//TODO: take this out...
	/*if (xval>0.1)
		xval=1.0;
	else if (xval<-0.1)
		xval=-1.0;
	if (yval>0.1)
		yval=1.0;
	else if (yval<-0.1)
		yval=-1.0;*/
	//m_robotDrive->ArcadeDrive(yval, -xval, m_rightStick->GetTrigger());			// drive with arcade style (use right stick
	m_kbot->m_leftJaguar->Set(-500.0);
	m_kbot->m_rightJaguar->Set(500.0);
}

// the goal of the macro is to weave back and forth, while still taking input from the operator
void RobotMacros::WeaveDrive()
{
	// Sinusoidal weave:
	//float leftMod = abs(sin(2.0*3.14*m_macroCycle/200.0));
	//float rightMod = abs(cos(2.0*3.14*m_macroCycle/200.0));
	// Triangular weave:
	//float leftMod = 1.0 - abs(1.0 - (m_macroCycle % 150)/75.0);
	//float rightMod = 1.0 - abs(1.0 - ((m_macroCycle+75) % 150)/75.0);
	//printf("cycle: %d sin: %5.2f  cos: %5.2f\n",m_macroCycle,leftMod,rightMod);
	//m_robotDrive->ArcadeDrive(m_rightStick,-leftMod,-rightMod, false);
	m_gyroDriveCtrl->setDesiredValue(30*(abs(1.0 - (m_macroCycle % 150)/75.0)-0.5));
	m_gyroDriveCtrl->setConstants(0.3, 0.0, 0.0); // 0.1
	float xval = m_rightStick->GetX();
	if (abs(m_rightStick->GetX()) > 0.10)
	{
		m_gyro->Reset();
	} else {
		xval += m_gyroDriveCtrl->calcPID(m_gyro->GetAngle());
	}
	m_robotDrive->ArcadeDrive(m_rightStick->GetY(), xval, m_rightStick->GetTrigger());			// drive with arcade style (use right stick

}

// Allow complete operator control.
void RobotMacros::OperatorControl()
{
	// TODO:  check various button states and
	// control the robot appropriately
}

// Spins the robot 90 degrees to the left or right depending on argument.
void RobotMacros::Turn(MacroNames dir)
{
	if (dir == mcTURNLEFT)
	{
		m_robotDrive->SetLeftRightMotorSpeeds(-1.0, 1.0);
	}
	else
	{
		m_robotDrive->SetLeftRightMotorSpeeds(1.0, -1.0);
	}
}

// Simply enough reverses the pickup. Could be used in relation to empty cells.
void RobotMacros::PickupReverse()
{
	//m_launcher->Drive(-1.0, true);
}

