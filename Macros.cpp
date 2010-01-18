#include "Macros.h"

RobotMacros::RobotMacros(KBot *kbot)
{
	m_kbot = kbot;
	m_macroState = mcNONE;
	m_macroCycle = 0;
	hopperPosition = 0;
	lastTrigger = false;
	triggerCycle = 1000;		// Starting at a big number makes sure the shooter
								// doesn't reverse when the robot is first turned on
	
	m_robotDrive = m_kbot->getRobotDrive();
	m_shooter = m_kbot->getShooter();
	m_pickup = m_kbot->getPickup();
	m_hopper = m_kbot->getHopper();
	m_leftStick = m_kbot->getLeftStick();
	m_rightStick = m_kbot->getRightStick();
	m_gyro = m_kbot->getGyro();
	m_camera = m_kbot->getCamera();
	m_gyroDriveCtrl = new KbotPID(0.01,0.0,0.0);
	m_gyro->Reset();
	m_gyroDriveCtrl->resetErrorSum();
	
	m_robotDrive->SetModifierConstant(TELE_WEAVE_PERCENT);
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
	m_robotDrive->ArcadeDrive(yval, xval, m_rightStick->GetTrigger());			// drive with arcade style (use right stick
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
	if (m_leftStick->GetRawButton(REVERSE_SHOOTER_BUTTON)){			// Reverse the shooter
		reverseShooterCycle++;
		if (reverseShooterCycle<200){
			m_shooter->Drive(-triggerCycle*0.002, true); // Gradually increase reverse speed
		} else {
			m_shooter->Drive(-0.5, true);
		}
	}
	else if (m_leftStick->GetRawButton(ADVANCE_SHOOTER_BUTTON)){			// Advance the shooter
		reverseShooterCycle++;
		if (reverseShooterCycle<200){
			m_shooter->Drive(triggerCycle*0.002, true); // Gradually increase forward speed
		} else {
			m_shooter->Drive(0.5, true);
		}
	} else {
		reverseShooterCycle = 0;
		bool trigger = m_leftStick->GetTrigger();
		if (trigger != lastTrigger) { 	// Change in trigger status
			if (trigger) {				// If it was just pressed
				triggerCycle = 0;
				// Stop the shooter
				m_shooter->Drive(0.0, true);
				// Back off the pickup
				m_pickup->Drive(-0.5,true);
			} else {					// If it was just released
				triggerCycle = 0;
				m_shooter->Drive(0.0, true);
				m_pickup->Drive(m_leftStick->GetY(), true);
			}
			lastTrigger = trigger; 
		} else { 						// No change in trigger
			if (trigger) {				// Trigger is still pressed
				triggerCycle++;
				if (triggerCycle<5) {
					// Brake the shooter by moving it back slightly
					m_shooter->Drive(-0.25, true);
					// Back off the pickup
					m_pickup->Drive(0.5,true);
				}
				else if (triggerCycle<CYCLES_TO_BACK_OFF_PICKUP) {
					// Stop the shooter
					m_shooter->Drive(0.0, true);
					// Back off the pickup
					m_pickup->Drive(0.5,true);
				} else {
					float shooterSpeed;
					// If override button pressed, or camera has lost connection, use Z-axis
					if (m_leftStick->GetRawButton(SHOOTER_SPEED_BUTTON) /*|| !CameraInitialized()*/)
					{
						shooterSpeed = m_leftStick->GetZ();
						if (shooterSpeed<SHOOT_MIN)
						{
							shooterSpeed=SHOOT_MIN;
						}
						printf("Shooter speed %5.4f\n",shooterSpeed);
					}
					else // Otherwise use auto shooter speed:
					{
						if ( m_camera->lockedOn())
						{
							//shooterSpeed = (m_camera->getTargetY()-CAM_MIN)*(SHOOT_MAX-SHOOT_MIN)/(CAM_MAX-CAM_MIN)+SHOOT_MIN;
							shooterSpeed = SHOOT_DEFAULT;
							printf("Auto shooter speed %5.2f; targetY= %5.4f\n",shooterSpeed,m_camera->getTargetY());
						}
						else // Lost camera lock; assume we are too close
						{
							shooterSpeed = SHOOT_MIN;
							printf("Lost camera lock; speed %5.2f\n",shooterSpeed);
						}
					}
					if (triggerCycle<CYCLES_TO_BACK_OFF_PICKUP+CYCLES_TO_SPEED_SHOOTER)
					{
						// Spin up the shooter
						m_shooter->Drive(shooterSpeed,true);
						// Stop the pickup
						m_pickup->Drive(0.0,true);
					} else {
						// Run the shooter and the pickup
						m_shooter->Drive(shooterSpeed,true);
						m_pickup->Drive(m_leftStick->GetY(),true);
					}
				}
			} else {					// Trigger still not pressed
				triggerCycle++;
				if (triggerCycle<75) {
					m_shooter->Drive(-triggerCycle*0.002, true); // Gradually increase braking
					m_pickup->Drive(m_leftStick->GetY(), true);
				} else {
					m_shooter->Drive(0.0, true);
					m_pickup->Drive(m_leftStick->GetY(), true);
				}
			}
		}
	}

	if (m_leftStick->GetRawButton(HOPPER_OPEN_BUTTON)) {
		hopperPosition += 0.01;
	}
	if (m_leftStick->GetRawButton(HOPPER_CLOSE_BUTTON)) {
		hopperPosition -= 0.01;
	}
	m_hopper->Drive(0.0,true);
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

