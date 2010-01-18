/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KBOTROBOTDRIVE_H_
#define KBOTROBOTDRIVE_H_

#include <stdlib.h>
#include <VxWorks.h>
#include "Base.h"
#include "KbotVPID.h"
#include "Encoder.h"
#include "CurrentSensor.h"

class SpeedController;
class GenericHID;

//You should add a comment...
#define MAX_ACCEL	0.0025 //0.0025
//You should add a comment...
#define MAX_TORQUE 300   //145
// Volts per speed unit
#define K_Jaguar	12

// Map encoder to approx range of -1.0 to 1.0
#define K_Encoder	2

/**
 * Utility class for handling Robot drive based on a definition of the motor configuration.
 * The robot drive class handles basic driving for a robot. Currently, 2 and 4 motor standard
 * drive trains are supported. In the future other drive types like swerve and meccanum might
 * be implemented. Motor channel numbers are passed supplied on creation of the class. Those are
 * used for either the Drive function (intended for hand created drive code, such as autonomous)
 * or with the Tank/Arcade functions intended to be used for Operator Control driving.
 */
class KbotRobotDrive
{
public:
	typedef enum
	{
		kLeftMotor = 0,
		kRightMotor = 1,
	} MotorType;

	KbotRobotDrive(UINT32 leftMotorChannel, UINT32 rightMotorChannel, float sensitivity = 0.5);
	virtual ~KbotRobotDrive();

	void Drive(float speed, float curve);
	void ArcadeDrive(GenericHID *stick, bool squaredInputs = true);
	void ArcadeDrive(GenericHID *stick, float leftWheelMod, float rightWheelMod, bool squaredInputs = true);
	void ArcadeDrive(float moveValue, float rotateValue, bool squaredInputs = true);
	void ArcadeDrive(float moveValue, float turnValue, float leftWheelMod, float rightWheelMod, bool squaredInputs = true);
	void SetLeftRightMotorSpeeds(float leftSpeed, float rightSpeed);
	void SetModifierConstant(float mod);
	void Stop();
	void SetLeftCurrentSensor(CurrentSensor *sens);
	void SetRightCurrentSensor(CurrentSensor *sens);
	void SetLeftEncoder(Encoder *enc);
	void SetRightEncoder(Encoder *enc);
	void SetTorqueLimit(bool limit);
	float getTorque();
	void setTorque(float t);
	void SetTime(UINT32 t);
	void SetPulseMode(bool en);

private:
	float Limit(float num);
	float TorqueLimit(float speed, float voltage, float current);
	float m_torque;

	static const INT32 kMaxNumberOfMotors = 2;
	
	bool m_torqueLimited;
	float m_sensitivity;
	float m_modifierConstant;
	bool m_deleteSpeedControllers;
	SpeedController *m_leftMotor;
	SpeedController *m_rightMotor;
	Encoder *m_leftEncoder;
	Encoder *m_rightEncoder;
	CurrentSensor *m_leftCurrentSensor;
	CurrentSensor *m_rightCurrentSensor;
	KbotVPID *m_leftPID;
	KbotVPID *m_rightPID;
	double m_lastLeftEncoder;
	double m_lastRightEncoder;
	UINT32 m_time;
	bool m_pulseEnabled;
	DISALLOW_COPY_AND_ASSIGN(KbotRobotDrive);
};

#endif

