/*----------------------------------------------------------------------------*/
/* Copyright (c) KCVI Robotics.	                                              */
/* Open Source Software - may be modified and shared by FRC teams.            */
/*----------------------------------------------------------------------------*/

#ifndef CAMERACONTROLLER_H_
#define CAMERACONTROLLER_H_

#include "WPILib.h"
#include "Vision2009/AxisCamera.h" 
//#include "Vision2009/BaeUtilities.h"
#include "Vision2009/FrcError.h"
#include "Vision2009/TrackAPI.h" 
//#include "Target.h" 

#include <stdlib.h>
#include <VxWorks.h>
#include "KbotPID.h"
//#include "AxisCamera.h"
//#include "FrcError.h"
//#include "TrackAPI.h"
#include <math.h>

class SpeedController;
class Servo;
class GenericHID;

/**
 * Utility class for handling the robot CameraController.
 */

// Camera tracking target size parameters
//									  0.00125  = 24 pixels at 160x120  (need to change in TrackAPI.cpp as well)
//                                    0.000625 = 48 pixels at 320x240
#define MIN_PARTICLE_TO_IMAGE_PERCENT 0.000625
#define MAX_PARTICLE_TO_IMAGE_PERCENT 100.0

#define CAMERA_X_SERVO_MIN 0.1
#define CAMERA_X_SERVO_MAX 0.9
#define CAMERA_Y_SERVO_MIN 0.36
#define CAMERA_Y_SERVO_MAX 0.6		// probably 0.62 is the largest needed

#define CAMERA_X_SCAN_STEP 0.025
#define CAMERA_Y_SCAN_STEP 0.08

class CameraController
{
public:

	CameraController();
	~CameraController(void);

	bool	FindGreen();
	bool	FindPink();
	int 	getLightX();
	int		getLightY();
	double 	getLightPct();
	float	getServoX();
	float	getServoY();
	void	SetServosTracking();
	void	SetServosScanning();
	void	DriveServos();
	void 	ResetServos();
	bool  	isCloseX();
	bool  	isCloseY();

private:
	//TrackingThreshold tdata;
	KbotPID *cameraYCtrl;
	KbotPID *cameraXCtrl;

	ParticleAnalysisReport par;

	Servo 	*m_camera_x;
	Servo 	*m_camera_y;
	bool	m_deleteSpeedControllers;
	
	float 	m_camera_x_pos;
	float 	m_camera_y_pos;
	float 	m_camera_dx;
	float 	m_camera_dy;
	int 	prev_lightX;
	int		prev_lightY;

	float Limit(float num,float min, float max);
};

#endif
