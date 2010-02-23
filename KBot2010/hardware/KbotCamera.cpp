/*----------------------------------------------------------------------------*/
/* Copyright (C) 2010 K-Botics. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "KbotCamera.h"

#include "Vision/AxisCamera.h"
#include "Vision/HSLImage.h"
#include "Target.h"

/**
 * KBotic camera, derived from AxisCamera
 * 
 */
KbotCamera::KbotCamera()
{
}

void KbotCamera::init()
{
	try
	{
		printf("Getting camera instance\n");
		AxisCamera &camera = AxisCamera::GetInstance();
		printf("Setting camera parameters\n");
		camera.WriteResolution(AxisCamera::kResolution_320x240);
		camera.WriteCompression(20);
		camera.WriteBrightness(50);  // was zero in WPI code was 50 in out old code
	}
	catch(...)
	{
		// do nothing--testing requires we run without camera
	}
}

vector<Target> KbotCamera::findTargets()
{
	vector<Target> vecTargets;
	AxisCamera& camera = AxisCamera::GetInstance();
	
	if (camera.IsFreshImage()) {
		// get the camera image
		HSLImage *pImage = camera.GetImage();

		// find FRC targets in the image
		vector<Target> vecTargets = Target::FindCircularTargets(pImage);
		delete pImage;
	}
	return vecTargets;
}


