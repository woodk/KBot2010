/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "KbotRobotDrive.h"

#include "GenericHID.h"
#include "Joystick.h"
#include "Jaguar.h"
#include "Utility.h"
#include "WPIStatus.h"
#include <math.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))

/*
 * Driving functions
 * These functions provide an interface to multiple motors that is used for C programming
 * The Drive(speed, direction) function is the main part of the set that makes it easy
 * to set speeds and direction independently in one call.
 */

/** Constructor for RobotDrive with 2 motors specified with channel numbers.
 * Set up parameters for a two wheel drive system where the
 * left and right motor pwm channels are specified in the call.
 * This call assumes Jaguars for controlling the motors.
 * @param leftMotorChannel The PWM channel number on the default digital module that drives the left motor.
 * @param rightMotorChannel The PWM channel number on the default digital module that drives the right motor.
 * @param sensitivity Effectively sets the turning sensitivity (or turn radius for a given value).
 */
KbotRobotDrive::KbotRobotDrive(UINT32 leftMotorChannel, UINT32 rightMotorChannel,
		float sensitivity)
{
	m_sensitivity = sensitivity;
	m_leftMotor = new Jaguar(leftMotorChannel);
	m_rightMotor = new Jaguar(rightMotorChannel);
	m_deleteSpeedControllers = true;

	m_leftPID = new KbotVPID(1.0, 0.0, 0.0);
	m_rightPID = new KbotVPID(1.0, 0.0, 0.0);
	
	m_lastLeftEncoder=0;
	m_lastRightEncoder=0;
	m_torqueLimited=false;
	m_torque = MAX_TORQUE;
}

/**
 * KbotRobotDrive destructor.
 * Deletes motor objects that were not passed in and created internally only.
 **/
KbotRobotDrive::~KbotRobotDrive()
{
	if (m_deleteSpeedControllers)
	{
		delete m_leftMotor;
		delete m_rightMotor;
	}
}

/**
 * Drive the motors at "speed" and "curve".
 *
 * The speed and curve are -1.0 to +1.0 values where 0.0 represents stopped and
 * not turning. The algorithm for adding in the direction attempts to provide a constant
 * turn radius for differing speeds.
 *
 * This function sill most likely be used in an autonomous routine.
 *
 * @param speed The forward component of the speed to send to the motors.
 * @param curve The rate of turn, constant for different forward speeds.
 */
void KbotRobotDrive::Drive(float speed, float curve)
{
	float leftSpeed, rightSpeed;

	if (curve < 0)
	{
		float value = log(-curve);
		float ratio = (value - m_sensitivity)/(value + m_sensitivity);
		if (ratio == 0) ratio =.0000000001;
		leftSpeed = speed / ratio;
		rightSpeed = speed;
	}
	else if (curve > 0)
	{
		float value = log(curve);
		float ratio = (value - m_sensitivity)/(value + m_sensitivity);
		if (ratio == 0) ratio =.0000000001;
		leftSpeed = speed;
		rightSpeed = speed / ratio;
	}
	else
	{
		leftSpeed = speed;
		rightSpeed = speed;
	}
	SetLeftRightMotorSpeeds(leftSpeed, rightSpeed);
}

/**
 * Arcade drive implements single stick driving.
 * Given a single Joystick, the class assumes the Y axis for the move value and the X axis
 * for the rotate value.
 * (Should add more information here regarding the way that arcade drive works.)
 * @param stick The joystick to use for Arcade single-stick driving. The Y-axis will be selected
 * for forwards/backwards and the X-axis will be selected for rotation rate.
 * @param squaredInputs If true, the sensitivity will be increased for small values
 */
void KbotRobotDrive::ArcadeDrive(GenericHID *stick, bool squaredInputs)
{
	// simply call the full-featured ArcadeDrive with the appropriate values
	ArcadeDrive(stick->GetY(), stick->GetX(), stick->GetTrigger());
}

/**
 * Same as above but with left wheel and right wheel modifiers
 * @param stick The joystick to use for Arcade single-stick driving. The Y-axis will be selected
 * for forwards/backwards and the X-axis will be selected for rotation rate.
 * @param squaredInputs If true, the sensitivity will be increased for small values
 */
void KbotRobotDrive::ArcadeDrive(GenericHID *stick, float leftWheelMod, float rightWheelMod, bool squaredInputs)
{
	// simply call the full-featured ArcadeDrive with the appropriate values
	ArcadeDrive(stick->GetY(), stick->GetX(), leftWheelMod, rightWheelMod, stick->GetTrigger());
}

/**
 * Arcade drive implements single stick driving.
 * This function lets you directly provide joystick values from any source.
 * @param moveValue The value to use for fowards/backwards
 * @param rotateValue The value to use for the rotate right/left
 * @param squaredInputs If set, increases the sensitivity at low speeds
 */
void KbotRobotDrive::ArcadeDrive(float moveValue, float turnValue, bool squaredInputs)
{
	// local variables to hold the computed PWM values for the motors
	float leftMotorSpeed;
	float rightMotorSpeed;

	moveValue = Limit(moveValue);
	turnValue = Limit(turnValue);

	if (squaredInputs)
	{
		// square the inputs (while preserving the sign) to increase fine control while permitting full power
		if (moveValue >= 0.0)
		{
			moveValue = (moveValue * moveValue);
		}
		else
		{
			moveValue = -(moveValue * moveValue);
		}
		if (turnValue >= 0.0)
		{
			turnValue = (turnValue * turnValue);
		}
		else
		{
			turnValue = -(turnValue * turnValue);
		}
	}
	
	leftMotorSpeed = moveValue - turnValue;
	rightMotorSpeed = moveValue + turnValue;
	if (moveValue>0) {		// Joystick backwards
		leftMotorSpeed += 2*moveValue*turnValue;
		rightMotorSpeed -= 2*moveValue*turnValue;
	}
	
	SetLeftRightMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
}

/**
 * Arcade drive implements single stick driving.
 * This function lets you directly provide joystick values from any source.
 * @param moveValue The value to use for fowards/backwards
 * @param rotateValue The value to use for the rotate right/left
 * @param squaredInputs If set, increases the sensitivity at low speeds
 */
void KbotRobotDrive::ArcadeDrive(float moveValue, float turnValue, float leftWheelMod, float rightWheelMod, bool squaredInputs)
{
	// local variables to hold the computed PWM values for the motors
	float leftMotorSpeed;
	float rightMotorSpeed;

	moveValue = Limit(moveValue);
	turnValue = Limit(turnValue);

	if (squaredInputs)
	{
		// square the inputs (while preserving the sign) to increase fine control while permitting full power
		if (moveValue >= 0.0)
		{
			moveValue = (moveValue * moveValue);
		}
		else
		{
			moveValue = -(moveValue * moveValue);
		}
		if (turnValue >= 0.0)
		{
			turnValue = (turnValue * turnValue);
		}
		else
		{
			turnValue = -(turnValue * turnValue);
		}
	}
	moveValue = Limit(moveValue);
	turnValue = Limit(turnValue);
	
	leftMotorSpeed = moveValue - turnValue;
	rightMotorSpeed = moveValue + turnValue;
	if (moveValue>0) {		// Joystick backwards
		leftMotorSpeed += 2*moveValue*turnValue;
		rightMotorSpeed -= 2*moveValue*turnValue;
	}

	moveValue = Limit(moveValue);
	turnValue = Limit(turnValue);
	
	leftMotorSpeed = leftMotorSpeed - m_modifierConstant*leftWheelMod;
	rightMotorSpeed = rightMotorSpeed - m_modifierConstant*rightWheelMod;
	
	SetLeftRightMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
}

/* Set the modifier constant for above */
void KbotRobotDrive::SetModifierConstant(float mod)
{
	m_modifierConstant = mod;
}

/** Set the speed of the right and left motors.
 * This is used once an appropriate drive setup function is called such as
 * TwoWheelDrive(). The motors are set to "leftSpeed" and "rightSpeed"
 * and includes flipping the direction of one side for opposing motors.
 * @param leftSpeed The speed to send to the left side of the robot.
 * @param rightSpeed The speed to send to the right side of the robot.
 */
void KbotRobotDrive::SetLeftRightMotorSpeeds(float leftSpeed, float rightSpeed)
{
	wpi_assert(m_leftMotor != NULL && m_rightMotor != NULL);
	
	leftSpeed = Limit(leftSpeed);
	rightSpeed = Limit(rightSpeed);

	if (m_torqueLimited)
	{
		m_leftPID->setDesiredValue(leftSpeed);
		double leftEncoder = m_leftEncoder->GetDistance();
		float leftSpeedOut = (leftEncoder - m_lastLeftEncoder)*K_Encoder;
		m_lastLeftEncoder = leftEncoder; 
		float leftSpeedInter = m_leftPID->calcPID(leftSpeedOut) + leftSpeed;
		float leftVoltage = leftSpeedInter * K_Jaguar;
		float leftCurrent = m_leftCurrentSensor->GetCurrent();
		float leftVoltageNew = TorqueLimit(leftSpeedInter, leftVoltage, leftCurrent);
		float leftSpeedNew = leftVoltageNew/K_Jaguar;
		//printf("Left:  spd=%5.2f enc.spd=%5.2f pid.spd=%5.2f volt=%5.2f curr=%5.2f new spd=%5.2f\n",leftSpeed,leftSpeedOut,leftSpeedInter,leftVoltage,leftCurrent,leftSpeedNew);
		//if ((m_time % 5<5) && m_pulseEnabled)
		//{
		//	m_leftMotor->Set(0.0);
		//}
		//else
		//{
			m_leftMotor->Set(Limit(leftSpeedNew));
		//}
	
		m_rightPID->setDesiredValue(rightSpeed);
		double rightEncoder = m_rightEncoder->GetDistance();
		float rightSpeedOut = (rightEncoder - m_lastRightEncoder)*K_Encoder;
		m_lastRightEncoder = rightEncoder; 
		float rightSpeedInter = m_rightPID->calcPID(rightSpeedOut) + rightSpeed;
		float rightVoltage = rightSpeedInter * K_Jaguar;
		float rightCurrent = m_rightCurrentSensor->GetCurrent();
		float rightVoltageNew = TorqueLimit(rightSpeedInter, rightVoltage, rightCurrent);
		float rightSpeedNew = rightVoltageNew/K_Jaguar;
		//printf("Right: spd=%5.2f enc.spd=%5.2f pid.spd=%5.2f volt=%5.2f curr=%5.2f new spd=%5.2f\n",rightSpeed,rightSpeedOut,rightSpeedInter,rightVoltage,rightCurrent,rightSpeedNew);
		//if ((m_time % 5<5) && m_pulseEnabled)
		//{
		//	m_rightMotor->Set(0.0);
		//}
		//else
		//{
			m_rightMotor->Set(Limit(-rightSpeedNew));
		//}
	}
	else
	{
		//if ((m_time % 5<5) && m_pulseEnabled)
		//{
		//	m_leftMotor->Set(0.0);
		//	m_rightMotor->Set(0.0);
		//}
		//else
		//{
			m_leftMotor->Set(leftSpeed);
			m_rightMotor->Set(-rightSpeed);
		//}
	}
}
void KbotRobotDrive::SetTorqueLimit(bool limit)
{
	m_torqueLimited = limit;
}
void KbotRobotDrive::Stop()
{
	m_leftMotor->Set(0.0);
	m_rightMotor->Set(0.0);
}

/**
 * Limit torque to MAX_TORQUE by limiting voltage to motors
 * @param speed The angular velocity input.
 * @param voltage The motor voltage.
 * @param current The motor current.
 * @return The angular velocity output with torque limited.
 */
float KbotRobotDrive::TorqueLimit(float speed, float voltage, float current)
{
	if (speed==0.0) {
		return speed;
	}
	float T = voltage * current / speed;

	if (T>m_torque){
		T=m_torque;
	} else if (T<-m_torque) {
		T=-m_torque;
	}
	return T * speed / current;
}

void KbotRobotDrive::SetLeftCurrentSensor(CurrentSensor *sens)
{
	m_leftCurrentSensor = sens;
}
void KbotRobotDrive::SetRightCurrentSensor(CurrentSensor *sens)
{
	m_rightCurrentSensor = sens;
}
void KbotRobotDrive::SetLeftEncoder(Encoder *enc)
{
	m_leftEncoder = enc;
}
void KbotRobotDrive::SetRightEncoder(Encoder *enc)
{
	m_rightEncoder = enc;
}
float KbotRobotDrive::getTorque()
{
	return m_torque;
}
void KbotRobotDrive::setTorque(float t)
{
	m_torque=t;
}

void KbotRobotDrive::SetTime(UINT32 t)
{
	m_time = t;
}
void KbotRobotDrive::SetPulseMode(bool en)
{
	m_pulseEnabled = en;
}

/**
 * Limit motor values to the -1.0 to +1.0 range.
 */
float KbotRobotDrive::Limit(float num)
{
	if (num > 1.0)
	{
		return 1.0;
	}
	if (num < -1.0)
	{
		return -1.0;
	}
	return num;
}
