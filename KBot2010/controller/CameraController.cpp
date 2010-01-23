/*----------------------------------------------------------------------------*/
/* Copyright (c) KCVI Robotics.	                                              */
/* Open Source Software - may be modified and shared by FRC teams.            */
/*----------------------------------------------------------------------------*/

#include "CameraController.h"

//#include "GenericHID.h"
#include "Servo.h"

TrackingThreshold tdataGreen; //should be in CameraController.h but doesn't work there
TrackingThreshold tdataPink;

/** Constructor for the CameraController. 
 * Set up parameters for the CameraController.
 */
CameraController::CameraController()
{
	// Camera servos and PID controllers
	m_camera_x_pos=0.5;
	m_camera_y_pos=0.5;
	m_camera_x = new Servo(5);
	m_camera_x->SetBounds(250,0,128,0,5);
	m_camera_x->Set(m_camera_x_pos);
	m_camera_y = new Servo(4);
	m_camera_y->SetBounds(250,0,128,0,5);
	m_camera_y->Set(m_camera_y_pos);
	//cameraXCtrl = new KbotPID(0.00004, 0.000001, 0.00002);
	cameraXCtrl = new KbotPID(0.00001, 0.0000001, 0.000002);
	cameraXCtrl->setDesiredValue(0.0);
	cameraXCtrl->setErrorEpsilon(2.0);
	cameraXCtrl->setMaxOutput(0.05);
	cameraXCtrl->setMinOutput(-0.05);
	//cameraYCtrl = new KbotPID(0.00003, 0.0000001, 0.000002);
	cameraYCtrl = new KbotPID(0.00001, 0.0000001, 0.000002);
	cameraYCtrl->setDesiredValue(0.0);
	cameraYCtrl->setErrorEpsilon(2.0);
	cameraYCtrl->setMaxOutput(0.05);
	cameraYCtrl->setMinOutput(-0.05);
	m_camera_dx=CAMERA_X_SCAN_STEP;
	m_camera_dy=-CAMERA_Y_SCAN_STEP;
	
	m_deleteSpeedControllers = true;

	// Start the camera.
	if (StartCameraTask(10, 0, k160x120, ROT_0) == -1)
	{
		printf("Failed to spawn camera task: %s\n",
				GetVisionErrorText(GetLastVisionError()));
	}
	else
	{
		printf("Camera started successfully\n");
	}

	// Set up camera tracking parameters.
	// Green (working)
	tdataGreen = GetTrackingData(GREEN, PASSIVE_LIGHT);
	tdataGreen.hue.minValue = 80;
	tdataGreen.hue.maxValue = 100;
	tdataGreen.saturation.minValue = 80;
	tdataGreen.saturation.maxValue = 200;
	tdataGreen.luminance.minValue = 80;
	tdataGreen.luminance.maxValue = 200;

	tdataPink = GetTrackingData(PINK, PASSIVE_LIGHT);
	tdataPink.hue.minValue = 230;//235
	tdataPink.hue.maxValue = 255;
	tdataPink.saturation.minValue = 80;
	tdataPink.saturation.maxValue = 200;
	tdataPink.luminance.minValue = 80;
	tdataPink.luminance.maxValue = 200;

	//Kevin's yellow tracking code 
	/*tdata = GetTrackingData(YELLOW, PASSIVE_LIGHT);
	tdata.hue.minValue = 20;
	tdata.hue.maxValue = 60;
	tdata.saturation.minValue = 150;
	tdata.saturation.maxValue = 255;
	tdata.luminance.minValue = 100;
	tdata.luminance.maxValue = 200;*/
	
	// ?????
	/*tdata.hue.minValue = 175;
	tdata.hue.maxValue = 181;
	tdata.saturation.minValue = 225;
	tdata.saturation.maxValue = 255;
	tdata.luminance.minValue = 196;
	tdata.luminance.maxValue = 212;*/


}

/**
 * CameraController destructor.
 * Deletes the motor object regardless of how it was allocated. Don't continue to use
 * any passed-in PWM objects after this class destructor is run.
 **/
CameraController::~CameraController(void)
{
	if (m_deleteSpeedControllers)
	{
		delete m_camera_x;
		delete m_camera_y;
	}
}

/**
 * Find a green object described by the parameters in the constructor. 
 * 
 * @return True if object is found; false if not.
 */
bool CameraController::FindGreen()
{
	return FindColor(IMAQ_HSL, &tdataGreen.hue, &tdataGreen.saturation, &tdataGreen.luminance, &par)
						&& par.particleToImagePercent < MAX_PARTICLE_TO_IMAGE_PERCENT
						&& par.particleToImagePercent > MIN_PARTICLE_TO_IMAGE_PERCENT;
}

/**
 * Find a pink object described by the parameters in the constructor. 
 * 
 * @return True if object is found; false if not.
 */
bool CameraController::FindPink()
{
	return FindColor(IMAQ_HSL, &tdataPink.hue, &tdataPink.saturation, &tdataPink.luminance, &par)
						&& par.particleToImagePercent < MAX_PARTICLE_TO_IMAGE_PERCENT
						&& par.particleToImagePercent > MIN_PARTICLE_TO_IMAGE_PERCENT;
}

/**
 * Get the X value of the object found. 
 * 
 * @return X value of found object.
 */
int CameraController::getLightX()
{
	return (int)(par.center_mass_x_normalized * 1000.0);
}

/**
 * Get the Y value of the object found. 
 * 
 * @return Y value of found object.
 */
int CameraController::getLightY()
{
	return (int)(par.center_mass_y_normalized * 1000.0);
}

/**
 * Get the particle to image percentage value of the object found. 
 * 
 * @return Pct value of found object.
 */
double CameraController::getLightPct()
{
	return par.particleToImagePercent;
}

/**
 * Get the position of the x-axis servo 
 * 
 * @return X-axis servo position.
 */
float CameraController::getServoX()
{
	return m_camera_x_pos;
}

/**
 * Get the position of the y-axis servo 
 * 
 * @return Y-axis servo position.
 */
float CameraController::getServoY()
{
	return m_camera_y_pos;
}

bool  CameraController::isCloseY()
{
	return prev_lightY<5;
}

bool  CameraController::isCloseX()
{
	return prev_lightX<5;
}


/**
 * Drive the servos toward the middle of the object using PID. 
 */
void CameraController::SetServosTracking()
{
	int lightX = getLightX();
	int lightY = getLightY();
	
	m_camera_dx = lightX>prev_lightX ? -CAMERA_X_SCAN_STEP : CAMERA_X_SCAN_STEP;
	m_camera_dy = lightY>prev_lightY ? -CAMERA_Y_SCAN_STEP : CAMERA_Y_SCAN_STEP;
	
	prev_lightX = lightX;
	prev_lightY = lightY;
	
	m_camera_x_pos-=cameraXCtrl->calcPID(lightX);
	m_camera_y_pos+=cameraYCtrl->calcPID(lightY);
}


/**
 * Scan the servos back and forth, up and down trying to find the object 
 */
void CameraController::SetServosScanning()
{
	m_camera_x_pos += m_camera_dx;
	if (m_camera_x_pos>CAMERA_X_SERVO_MAX or m_camera_x_pos<CAMERA_X_SERVO_MIN)
	{
		m_camera_dx=-m_camera_dx;
		m_camera_y_pos += m_camera_dy;
		if (m_camera_y_pos>CAMERA_Y_SERVO_MAX or m_camera_y_pos<CAMERA_Y_SERVO_MIN)
		{
			m_camera_dy=-m_camera_dy;
		}					
	}
}

/**
 * Moves servos to the positions set by SetServosTracking or SetServosScanning 
 */
void CameraController::DriveServos()
{
	m_camera_x_pos=Limit(m_camera_x_pos, CAMERA_X_SERVO_MIN, CAMERA_X_SERVO_MAX);
	m_camera_y_pos=Limit(m_camera_y_pos, CAMERA_Y_SERVO_MIN, CAMERA_Y_SERVO_MAX);
	m_camera_x->Set(m_camera_x_pos);
	m_camera_y->Set(m_camera_y_pos);
}

/**
 * Set the servos to point forward 
 */
void CameraController::ResetServos()
{
	m_camera_x->Set(0.5);
	m_camera_y->Set(0.5);
}

float CameraController::Limit(float num,float min, float max)
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
