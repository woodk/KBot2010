#include "RobotFactory.h"

#include "Joystick.h"
#include "Gyro.h"
#include "Encoder.h"
#include "DigitalInput.h"
#include "Solenoid.h"
#include "CANJaguar.h"
#include "KBotDrive.h"
#include "DriverStation.h"
#include "Relay.h"
#include "Kicker.h"
#include "KBotCamera.h"

Joystick* RobotFactory::BuildJoystick(UINT32 nNumber)
{
	return new Joystick(nNumber);
}
	
Gyro* RobotFactory::BuildGyro(UINT32 channel)
{
	return new Gyro(channel);
}

Encoder* RobotFactory::BuildEncoder(UINT32 aChannel, UINT32 bChannel, bool bReverseDirection)
{
	return new Encoder(aChannel, bChannel, bReverseDirection);
}

DigitalInput* RobotFactory::BuildDigitalInput(UINT32 channel)
{
	return new DigitalInput(channel);
}

Solenoid* RobotFactory::BuildSolenoid(UINT32 slot, UINT32 channel)
{
	return new Solenoid(slot, channel);
}

Solenoid* RobotFactory::BuildSolenoid(UINT32 channel)
{
	return new Solenoid(channel);
}

CANJaguar* RobotFactory::BuildCANJaguar(UINT8 deviceNumber, CANJaguar::ControlMode controlMode)
{
	return new CANJaguar(deviceNumber, controlMode);
}

KBotDrive* RobotFactory::BuildKBotDrive(SpeedController *frontLeftMotor, SpeedController *rearLeftMotor,
			SpeedController *frontRightMotor, SpeedController *rearRightMotor,
			float sensitivity)
{
	return new KBotDrive(frontLeftMotor, rearLeftMotor,
			frontRightMotor, rearRightMotor,
			sensitivity);
}

DriverStation* RobotFactory::BuildDriverStation()
{
	return DriverStation::GetInstance();;
}

Relay* RobotFactory::BuildRelay(INT32 channel, Relay::Direction direction)
{
	return new Relay(channel, direction);
}

Kicker* RobotFactory::BuildKicker(KBot *kbot, int kickerOutChannel, int kickerInChannel, int electromagnetChannel)
{
	return new Kicker(kbot, kickerOutChannel, kickerInChannel, electromagnetChannel, this);
}

KbotCamera* RobotFactory::BuildKbotCamera()
{
	return new KbotCamera();
}
