/*----------------------------------------------------------------------------*/
/* Copyright (c) KBotics 2010. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams.            */
/*----------------------------------------------------------------------------*/

#include "KBotDrive.h"
#include "GenericHID.h"

	KBotDrive::KBotDrive(UINT32 leftMotorChannel, UINT32 rightMotorChannel, float sensitivity) : RobotDrive(leftMotorChannel, rightMotorChannel, sensitivity){;}
	KBotDrive::KBotDrive(UINT32 frontLeftMotorChannel, UINT32 rearLeftMotorChannel,
				UINT32 frontRightMotorChannel, UINT32 rearRightMotorChannel, float sensitivity) : RobotDrive(frontLeftMotorChannel, rearLeftMotorChannel,	frontRightMotorChannel, rearRightMotorChannel, sensitivity){;}
	KBotDrive::KBotDrive(SpeedController *leftMotor, SpeedController *rightMotor, float sensitivity) : RobotDrive(leftMotor, rightMotor, sensitivity){;}
	KBotDrive::KBotDrive(SpeedController &leftMotor, SpeedController &rightMotor, float sensitivity) : RobotDrive(leftMotor, rightMotor, sensitivity){;}
	KBotDrive::KBotDrive(SpeedController *frontLeftMotor, SpeedController *rearLeftMotor,
				SpeedController *frontRightMotor, SpeedController *rearRightMotor,
				float sensitivity) : RobotDrive(frontLeftMotor, rearLeftMotor, frontRightMotor, rearRightMotor, sensitivity){;}
	KBotDrive::KBotDrive(SpeedController &frontLeftMotor, SpeedController &rearLeftMotor,
				SpeedController &frontRightMotor, SpeedController &rearRightMotor,
				float sensitivity) : RobotDrive(frontLeftMotor, rearLeftMotor, frontRightMotor, rearRightMotor, sensitivity){;}

	
	/**
	 * Arcade drive implements single stick driving.
	 * Given a single Joystick, the class assumes the Y axis for the move value and the X axis
	 * for the rotate value.
	 * (Should add more information here regarding the way that arcade drive works.)
	 * @param stick The joystick to use for Arcade single-stick driving. The Y-axis will be selected
	 * for forwards/backwards and the X-axis will be selected for rotation rate.
	 * @param squaredInputs If true, the sensitivity will be increased for small values
	 */
	void KBotDrive::ArcadeDrive(GenericHID *stick, bool squaredInputs)
	{
		// simply call the full-featured ArcadeDrive with the appropriate values
		ArcadeDrive(stick->GetY(), stick->GetX(), stick->GetTrigger());
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
	void KBotDrive::ArcadeDrive(GenericHID &stick, bool squaredInputs)
	{
		// simply call the full-featured ArcadeDrive with the appropriate values
		ArcadeDrive(stick.GetY(), stick.GetX(), stick.GetTrigger());
	}

	/**
	 * Arcade drive implements single stick driving.
	 * Given two joystick instances and two axis, compute the values to send to either two
	 * or four motors.
	 * @param moveStick The Joystick object that represents the forward/backward direction
	 * @param moveAxis The axis on the moveStick object to use for fowards/backwards (typically Y_AXIS)
	 * @param rotateStick The Joystick object that represents the rotation value
	 * @param rotateAxis The axis on the rotation object to use for the rotate right/left (typically X_AXIS)
	 * @param squaredInputs Setting this parameter to true increases the sensitivity at lower speeds
	 */
	void KBotDrive::ArcadeDrive(GenericHID* moveStick, UINT32 moveAxis,
									GenericHID* rotateStick, UINT32 rotateAxis,
									bool squaredInputs)
	{
		float moveValue = moveStick->GetRawAxis(moveAxis);
		float rotateValue = rotateStick->GetRawAxis(rotateAxis);

		ArcadeDrive(moveValue, rotateValue, squaredInputs);
	}

	/**
	 * Arcade drive implements single stick driving.
	 * Given two joystick instances and two axis, compute the values to send to either two
	 * or four motors.
	 * @param moveStick The Joystick object that represents the forward/backward direction
	 * @param moveAxis The axis on the moveStick object to use for fowards/backwards (typically Y_AXIS)
	 * @param rotateStick The Joystick object that represents the rotation value
	 * @param rotateAxis The axis on the rotation object to use for the rotate right/left (typically X_AXIS)
	 * @param squaredInputs Setting this parameter to true increases the sensitivity at lower speeds
	 */

	void KBotDrive::ArcadeDrive(GenericHID &moveStick, UINT32 moveAxis,
									GenericHID &rotateStick, UINT32 rotateAxis,
									bool squaredInputs)
	{
		float moveValue = moveStick.GetRawAxis(moveAxis);
		float rotateValue = rotateStick.GetRawAxis(rotateAxis);

		ArcadeDrive(moveValue, rotateValue, squaredInputs);
	}

	/**
	 * Arcade drive implements single stick driving.
	 * This function lets you directly provide joystick values from any source.
	 * @param moveValue The value to use for fowards/backwards
	 * @param rotateValue The value to use for the rotate right/left
	 * @param squaredInputs If set, increases the sensitivity at low speeds
	 */
	void KBotDrive::ArcadeDrive(float moveValue, float rotateValue, bool squaredInputs)
	{
		// local variables to hold the computed PWM values for the motors
		float leftMotorSpeed;
		float rightMotorSpeed;

		moveValue = Limit(moveValue);
		rotateValue = Limit(rotateValue);

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
			if (rotateValue >= 0.0)
			{
				rotateValue = (rotateValue * rotateValue);
			}
			else
			{
				rotateValue = -(rotateValue * rotateValue);
			}
		}
		
		leftMotorSpeed = moveValue - rotateValue;
		rightMotorSpeed = moveValue + rotateValue;
		// Modify reverse so that back-left goes back-left and back-right goes back-right
		if (moveValue<0) {		// Joystick backwards (NOTE: < not > for new robot)
			leftMotorSpeed += 2*moveValue*rotateValue;
			rightMotorSpeed -= 2*moveValue*rotateValue;
		}
		
		SetLeftRightMotorSpeeds(leftMotorSpeed, rightMotorSpeed);

	}
