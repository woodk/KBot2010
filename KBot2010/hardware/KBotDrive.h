/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KBOTDRIVE_H_
#define KBOTDRIVE_H_

#include "RobotDrive.h"

/**
 * description
 */
class KBotDrive : public RobotDrive
{
public:
	KBotDrive(UINT32 leftMotorChannel, UINT32 rightMotorChannel);
	KBotDrive(UINT32 frontLeftMotorChannel, UINT32 rearLeftMotorChannel,
				UINT32 frontRightMotorChannel, UINT32 rearRightMotorChannel);
	KBotDrive(SpeedController *leftMotor, SpeedController *rightMotor);
	KBotDrive(SpeedController &leftMotor, SpeedController &rightMotor);
	KBotDrive(SpeedController *frontLeftMotor, SpeedController *rearLeftMotor,
				SpeedController *frontRightMotor, SpeedController *rearRightMotor);
	KBotDrive(SpeedController &frontLeftMotor, SpeedController &rearLeftMotor,
				SpeedController &frontRightMotor, SpeedController &rearRightMotor);
	void ArcadeDrive(GenericHID *stick, bool squaredInputs = true);
	void ArcadeDrive(GenericHID &stick, bool squaredInputs = true);
	void ArcadeDrive(GenericHID *moveStick, UINT32 moveChannel, GenericHID *rotateStick, UINT32 rotateChannel, bool squaredInputs = true);
	void ArcadeDrive(GenericHID &moveStick, UINT32 moveChannel, GenericHID &rotateStick, UINT32 rotateChannel, bool squaredInputs = true);
	void ArcadeDrive(float moveValue, float rotateValue, bool squaredInputs = true);

	float getLeftSpeed() {return m_fLeftSpeed;}
	float getRightSpeed() {return m_fRightSpeed;}
	
	void initSpeeds()
	{
		m_fLeftSpeed = 0;
		m_fRightSpeed = 0;
	}
	
protected:
	
	float m_fLeftSpeed;
	float m_fRightSpeed;
	
private:
	DISALLOW_COPY_AND_ASSIGN(KBotDrive);
};

#endif

