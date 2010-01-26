/*----------------------------------------------------------------------------*/
/* Copyright (C) 2010 K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "KbotCamera.h"

#include "Vision/AxisCamera2010.h"
#include "Vision/HSLImage.h"
#include "Target.h"

/**
 * KBotic camera, derived from AxisCamera
 * 
 */
KbotCamera::KbotCamera()
{
}

void KbotCamera::teleopInit()
{
	AxisCamera& camera = AxisCamera::getInstance();
	camera.writeResolution(k320x240);
	camera.writeBrightness(50);	
}

vector<Target> KbotCamera::findTargets()
{
	AxisCamera& camera = AxisCamera::getInstance();
		
	vector<Target> vecTargets;
	if (camera.freshImage()) {
		// get the camera image
		ColorImage *pImage = camera.GetImage();
		
		vecTargets = Target::FindCircularTargets(pImage);
		delete pImage;
		
	}
	
	return vecTargets;
}


