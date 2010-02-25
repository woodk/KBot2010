/*----------------------------------------------------------------------------*/
/* Copyright (c) KCVI Robotics.	                                              */
/* Open Source Software - may be modified and shared by FRC teams.            */
/*----------------------------------------------------------------------------*/

#ifndef ROBOTCONTROLLER_H_
#define ROBOTCONTROLLER_H_

#include <stdlib.h>
#include <VxWorks.h>
#include "KbotPID.h"
#include "FrcError.h"
#include <math.h>
#include "Gyro.h"
#include "KBotDrive.h"

//class SpeedController;
//class Servo;
//class GenericHID;

/**
 * Utility class for handling the robot RobotController. 
 */

#define CAMERA_Y_TARGET	-500
#define ANGLE_ADJUST	0.6
class RobotController
{
public:

	RobotController(KBotDrive *robotDrive);
	~RobotController(void);
	void TurnLeft(float angle);
	void TurnRight(float angle);
	float GetAngle();
	void DriveToLight(int xPos, int yPos);
	void Reset();
	
private:
	KbotPID		*gyroCtrl,*lightXCtrl,*lightYCtrl;
	Gyro 		*m_gyro;
	KBotDrive	*m_robotDrive;
	
	float Limit(float num,float min, float max);
};

#endif
