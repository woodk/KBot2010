/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KBOTCAMERA_H_
#define KBOTCAMERA_H_

#include "WPILib.h"
#include "Target.h"

#define MINIMUM_SCORE 0.01

/**
 * High level hardware class for controlling all camera functions.
Wraps singleton AxisCamera
 */
class KbotCamera
{
public:
	KbotCamera();
	
	//! set up camera
	void init();
	
	//! find targets using standard algorithms
	vector<Target> findTargets();
	
private:
	
	Timer m_timer;
	
};

#endif
