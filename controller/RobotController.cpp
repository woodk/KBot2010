/*----------------------------------------------------------------------------*/
/* Copyright (c) KCVI Robotics.	                                              */
/* Open Source Software - may be modified and shared by FRC teams.            */
/*----------------------------------------------------------------------------*/

#include "RobotController.h"

//#include "GenericHID.h"
#include "Servo.h"

/** Constructor for the RobotController. 
 * Set up parameters for the RobotController.
 */
RobotController::RobotController(RobotDrive *robotDrive)
{
	m_robotDrive = robotDrive;

	// Gyro
	m_gyro = new Gyro(1);
	m_gyro->SetSensitivity(0.007); // 7 mV/deg/s
	m_gyro->Reset();
	
	// PID controllers
	// Gyro:  1 degree - 1000
	gyroCtrl = new KbotPID(0.00002, 0.0, 0.0001);
	gyroCtrl->setDesiredValue(0);
	gyroCtrl->setErrorEpsilon(10);
	gyroCtrl->setMaxOutput(0.5);
	gyroCtrl->setMinOutput(-0.5);
	
	lightXCtrl = new KbotPID(0.0005, 0.0, 0.001);
	lightXCtrl->setDesiredValue(0);
	lightXCtrl->setErrorEpsilon(10);
	lightXCtrl->setMaxOutput(0.5);
	lightXCtrl->setMinOutput(-0.5);
	
	lightYCtrl = new KbotPID(0.0005, 0.0, 0.0001);
	lightYCtrl->setDesiredValue(0);
	lightYCtrl->setErrorEpsilon(10);
	lightYCtrl->setMaxOutput(0.5);
	lightYCtrl->setMinOutput(-0.5);
}

/**
 * RobotController destructor.
 * Deletes the motor object regardless of how it was allocated. Don't continue to use
 * any passed-in PWM objects after this class destructor is run.
 **/
RobotController::~RobotController(void)
{
}

/** Reset the gyro. 
 * 
 */
void RobotController::Reset()
{
	m_gyro->Reset();
}

/** Turn left. 
 * @param angle the angle to turn, from -1.0 to 1.0 (-90deg to 90deg)
 */
void RobotController::TurnLeft(float angle)
{
	gyroCtrl->setDesiredValue((int)(-90000*angle*ANGLE_ADJUST));
	float drive=gyroCtrl->calcPID((int)(1000*m_gyro->GetAngle()));
	//printf("drive = %5.2f    gyro = %5.2f\n",drive,m_gyro->GetAngle());
	m_robotDrive->SetLeftRightMotorSpeeds(-drive,drive);
}

/** Turn right. 
 * @param angle the angle to turn, from -1.0 to 1.0 (-90deg to 90deg)
 */
void RobotController::TurnRight(float angle)
{
	gyroCtrl->setDesiredValue((int)(90000*angle*ANGLE_ADJUST));
	float drive=gyroCtrl->calcPID((int)(1000*m_gyro->GetAngle()));
	printf("drive = %5.2f    gyro = %5.2f\n",drive,m_gyro->GetAngle());
	m_robotDrive->SetLeftRightMotorSpeeds(-drive,drive);
}

/** Drive toward light. 
 * @param xPos
 * @param cameraAngle
 */
void RobotController::DriveToLight(int xPos, int yPos)
{
	//printf("xPos = %d    yPos = %d\n", xPos, yPos);

	lightYCtrl->setDesiredValue(CAMERA_Y_TARGET);
	float drive=lightYCtrl->calcPID(yPos);

	lightXCtrl->setDesiredValue(0);
	float turn=lightXCtrl->calcPID(xPos);
	
	//printf("drive = %5.2f    turn = %5.2f\n",drive,turn);
	m_robotDrive->SetLeftRightMotorSpeeds(-drive+turn,-drive-turn);
}


/** Get the angle that has been turned already. 
 * @return Angle that has been turned, from -1.0 to 1.0 (-90deg to 90deg)
 */
float RobotController::GetAngle()
{
	return m_gyro->GetAngle()/90.0;
}

float RobotController::Limit(float num,float min, float max)
{
	if (num > max)
	{
		return max;
	}
	if (num < min)
	{
		return min;
	}
	return num;
}
