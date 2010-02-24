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
	m_timer.Start();
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
		vecTargets = Target::FindCircularTargets(pImage);
		delete pImage;
		
		if ((vecTargets.size() == 0) || (vecTargets[0].m_score < MINIMUM_SCORE))
		{
			// no targets found. Make sure the first one in the list is 0,0
			// since the dashboard program annotates the first target in green
			// and the others in magenta. With no qualified targets, they'll all
			// be magenta.
			Target nullTarget;
			nullTarget.m_majorRadius = 0.0;
			nullTarget.m_minorRadius = 0.0;
			nullTarget.m_score = 0.0;
			if (vecTargets.size() == 0)
				vecTargets.push_back(nullTarget);
			else
				vecTargets.insert(vecTargets.begin(), nullTarget);
		}
	}
	return vecTargets;
}
