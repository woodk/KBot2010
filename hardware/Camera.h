/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.								  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef CAMERA_H
#define CAMERA_H

#include "WPILib.h"
#include "Vision2009/AxisCamera.h" 
#include "Vision2009/BaeUtilities.h"
#include "Vision2009/FrcError.h"
#include "Vision2009/TrackAPI.h" 
#include "Target.h" 


//#include "AxisCamera.h" 
//#include "BaeUtilities.h"
//#include "TrackAPI.h"
//#include "Target.h"
#include "KBot.h"
//#include "FrcError.h"

class KBot;

#define PI 3.14159265358979
		
// for 160x120, 50 pixels = .38 %
// for 320x240, 50 pixels = 0.065%
#define MIN_PARTICLE_TO_IMAGE_PERCENT 0.1		// target is too small was 0.25 
#define MAX_PARTICLE_TO_IMAGE_PERCENT 40.0		// target is too close
#define MIN_SERVO_VERTICAL 0.4375//0.5	// UP
#define MAX_SERVO_VERTICAL 0.578125 //0.640625 // DOWN = 41/64 Should be a binary decimal

#define SERVO_STEPS  4

/**
 * Robot camera control
 */
class Camera
{
public:
	explicit Camera(KBot *kbot);
	virtual ~Camera();
	void CameraInit();
	void CameraReset();
	void CameraPeriodic(bool enabled);
	int tooClose(ParticleAnalysisReport* par);
	int bigEnough(ParticleAnalysisReport* par);
	void setServoPosition(float normalizedHorizontal, float normalizedVertical);
	void adjustServoPosition(float normDeltaHorizontal, float normDeltaVertical);
	float getTargetX();
	float getTargetY();
	bool lockedOn();
	double getParticleSize();
	void setGreenPosition(SecondColorPosition pos);
	float getDistanceToTarget();
	void setPanEnabled(bool pan);	
	void setFixedServoPos(float pos);	
	
private:
	KBot *m_kbot;
	bool m_cameraInitialized;
	float m_panDir;
	bool m_panEnabled;
	float m_servoPos;
	Servo *horizontalServo;  		// first servo object
	Servo *verticalServo;			// second servo object
	float horizontalDestination;	// servo destination (0.0-1.0)
	float verticalDestination;		// servo destination (0.0-1.0)
	float horizontalPosition, verticalPosition;	// current servo positions
	float servoDeadband;			// percentage servo delta to trigger move
	int framesPerSecond;			// number of camera frames to get per second
	float panControl;				// to slow down pan
	double sinStart;				// control where to start the sine wave input for pan
	TrackingThreshold td1, td2;		// color thresholds
	
	SecondColorPosition m_greenPosition;	// is green ABOVE or BELOW pink
	ParticleAnalysisReport par1, par2;		// particle analysis reports

	// search variables 
	bool foundColor; 
	double savedImageTimestamp;
	bool staleImage; 
	// initialize pan variables
	// incremental tasking toward dest (-1.0 to 1.0)
	float incrementH, incrementV;
	// pan needs a 1-up number for each call
	int panIncrement;
	
	void myPanForTarget(Servo *panServo, float minPos, float maxPos);
};
#endif

