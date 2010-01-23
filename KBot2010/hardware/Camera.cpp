/*----------------------------------------------------------------------------*/
/* Copyright (c) K-Botics 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Camera.h"
#include "Math.h"
/**
 * Camera constructor.
 */
Camera::Camera(KBot *kbot)
{
	m_kbot = kbot;
	m_cameraInitialized=false;
	m_servoPos = MAX_SERVO_VERTICAL;
	m_panDir = 1.0;
	// remember to use jumpers on the sidecar for the Servo PWMs
	horizontalServo = new Servo(10); 		// create horizontal servo on PWM 9
	verticalServo = new Servo(9);			// create vertical servo on PWM 10
	horizontalServo->Set(0.5);
	verticalServo->Set(0.65);
	servoDeadband = 0.001;					// move if > this amount 
	framesPerSecond = 20;	//was 15				// number of camera frames to get per second
	sinStart = 0.0;							// control where to start the sine wave for pan
	memset(&par1,0,sizeof(ParticleAnalysisReport));			// initialize particle analysis report
	memset(&par2,0,sizeof(ParticleAnalysisReport));			// initialize particle analysis report

	/* image data for tracking - override default parameters if needed */
	/* recommend making PINK the first color because GREEN is more 
	 * subsceptible to hue variations due to lighting type so may
	 * result in false positives */
	// PINK
	sprintf (td1.name, "PINK");
	/* Competition values:
	td1.hue.minValue = 220;   
	td1.hue.maxValue = 255;  
	td1.saturation.minValue = 75;   
	td1.saturation.maxValue = 255;      
	td1.luminance.minValue = 85;  
	td1.luminance.maxValue = 255;*/
	td1.hue.minValue = 235;   
	td1.hue.maxValue = 250;  
	td1.saturation.minValue = 80;   
	td1.saturation.maxValue = 255;      
	td1.luminance.minValue = 50;  
	td1.luminance.maxValue = 255;
	// GREEN
	sprintf (td2.name, "GREEN");
	/* Competition values:
	td2.hue.minValue = 55;   
	td2.hue.maxValue = 125;  
	td2.saturation.minValue = 58;   
	td2.saturation.maxValue = 255;    
	td2.luminance.minValue = 92;  
	td2.luminance.maxValue = 255;*/
	td2.hue.minValue = 80;   
	td2.hue.maxValue = 105;  
	td2.saturation.minValue = 80;   
	td2.saturation.maxValue = 255;    
	td2.luminance.minValue = 50;  
	td2.luminance.maxValue = 255;
	
	// Default team designation is Green over Pink
	m_greenPosition = ABOVE;
	
	/* start the CameraTask	 */
	if (StartCameraTask(framesPerSecond, 0, k320x240, ROT_0) == -1) {
		printf( "Failed to spawn camera task; exiting. Error code %s\n", GetVisionErrorText(GetLastVisionError()) );
	}
	else {
		printf("Camera task spawned\n");
	}
}

/**
 * Initialize Camera object.
 */
void Camera::CameraInit()
{
	if (!m_cameraInitialized)
	{
		// initialize position and destination variables
		// position settings range from -1 to 1
		// setServoPositions is a wrapper that handles the conversion to range for servo 
		horizontalDestination = 0.0;		// final destination range -1.0 to +1.0
		verticalDestination = 0.0;
		
		// pan needs a 1-up number for each call
		panIncrement = 0;							
		
		// current position range -1.0 to +1.0
		horizontalPosition = RangeToNormalized(horizontalServo->Get(),1);	
		verticalPosition = RangeToNormalized(verticalServo->Get(),1);			
		
		// set servos to start at center position
		setServoPosition(horizontalDestination, verticalDestination);
		// search variables 
		foundColor = false; 
		savedImageTimestamp = 0.0;
		staleImage = false;
		m_cameraInitialized=true;
	}
}

/**
 * Reset Camera by re-Initializing
 */
void Camera::CameraReset()
{
	m_cameraInitialized=false;
	CameraInit();
}

/**
 * Free any resources used.
 */
Camera::~Camera()
{
}

void Camera::CameraPeriodic(bool enabled)
{
	/*if (!CameraInitialized())
	{
		printf("****** Camera not initialized ************\n");
		m_kbot->getDriverStation()->SetDigitalOut(DS_CAMERA_LOST,true);
		m_cameraInitialized=false;
		CameraInit();
	}
	else
	{*/
		m_kbot->getDriverStation()->SetDigitalOut(DS_CAMERA_LOST,false);
	//}
	// calculate gimbal position based on colors found 
	if ( FindTwoColors(td1, td2, m_greenPosition, &par1, &par2) ){
		//PrintReport(&par2);
		foundColor = true;
		// reset pan		
		panIncrement = 0;  		
		if (par1.imageTimestamp == savedImageTimestamp) {
			// This image has been processed already, 
			// so don't do anything for this loop 
			staleImage = true;
			printf("STALE IMAGE\n");
			
		} else {
			// The target was recognized
			// save the timestamp
			staleImage = false;
			savedImageTimestamp = par1.imageTimestamp;	
			//printf("image timetamp: %lf\n", savedImageTimestamp);

			// get center of target 
			// Average the color two particles to get center x & y of combined target
			horizontalDestination = (par1.center_mass_x_normalized + par2.center_mass_x_normalized) / 2;	
			verticalDestination = (par1.center_mass_y_normalized + par2.center_mass_y_normalized) / 2;
			//printf("found target at %f, %f\n",horizontalDestination,verticalDestination);
		}
	} else {  // need to pan 
		foundColor = false;
	} 
						
	if(foundColor && !staleImage) {	
		/* Move the servo a bit each loop toward the destination.
		 * Alternative ways to task servos are to move immediately vs.
		 * incrementally toward the final destination. Incremental method
		 * reduces the need for calibration of the servo movement while
		 * moving toward the target.
		 */
		incrementH = horizontalDestination - horizontalPosition;
		// you may need to reverse this based on your vertical servo installation
		//incrementV = verticalPosition - verticalDestination;
		incrementV = verticalDestination - verticalPosition;
		if (enabled) {
			adjustServoPosition( incrementH, incrementV );}  
		
		//printf ("** %s & %s found: Y value: %f** part size %f \n",td1.name, td2.name, verticalPosition,getParticleSize());
		//printf ("** %s & %s found: ServoDest: x: %f  y: %f ServoPos: x: %f y: %f** \n", 
		//		td1.name, td2.name, horizontalDestination, verticalDestination, horizontalPosition, verticalPosition);	
		
	} else { //if (!staleImage) {  // new image, but didn't find two colors
		
		// adjust sine wave for panning based on last movement direction
		if(verticalDestination > 0.0)	{ sinStart = PI/2.0; }
		else { sinStart = -PI/2.0; }

		/* pan to find color after a short wait to settle servos
		 * panning must start directly after panInit or timing will be off */				
		if (panIncrement == 3) {
			panInit(4.0);		// number of seconds for a pan
		}
		else if (panIncrement > 3) {	
			if (enabled)
				myPanForTarget(verticalServo, MIN_SERVO_VERTICAL, MAX_SERVO_VERTICAL);	
		}
		panIncrement++;		

		//printf ("** %s and %s not found \n", td1.name, td2.name);
	}  // end if found color
}

/** Simple test to see if the color is taking up too much of the image */
int tooClose(ParticleAnalysisReport* par) {
	if (par->particleToImagePercent > MAX_PARTICLE_TO_IMAGE_PERCENT)
		return 1;
	return 0;
}

/** Simple test to see if the color is large enough */
int bigEnough(ParticleAnalysisReport* par) {
	if (par->particleToImagePercent < MIN_PARTICLE_TO_IMAGE_PERCENT)
		return 0;
	return 1;
}

double Camera::getParticleSize()
{
	return (par1.particleToImagePercent+par2.particleToImagePercent)/2;
}

/**
 * Set servo positions (0.0 to 1.0) translated from normalized values (-1.0 to 1.0). 
 * 
 * @param normalizedHorizontal Pan Position from -1.0 to 1.0.
 * @param normalizedVertical Tilt Position from -1.0 to 1.0.
 */
void Camera::setServoPosition(float normalizedHorizontal, float normalizedVertical)	{

	float servoH = NormalizeToRange(normalizedHorizontal);
	float servoV = NormalizeToRange(normalizedVertical);
	
	float currentH = horizontalServo->Get();		
	float currentV = verticalServo->Get();
	
	/* make sure the movement isn't too small */
	if ( fabs(servoH - currentH) > servoDeadband ) {
		horizontalServo->Set( servoH );
		/* save new normalized horizontal position */
		horizontalPosition = RangeToNormalized(servoH, 1);
	}
	if ( fabs(servoV - currentV) > servoDeadband ) {
		verticalServo->Set( servoV );
		verticalPosition = RangeToNormalized(servoV, 1);
	}
}	

/**
 * Adjust servo positions (0.0 to 1.0) translated from normalized values (-1.0 to 1.0). 
 * 
 * @param normalizedHorizontal Pan adjustment from -1.0 to 1.0.
 * @param normalizedVertical Tilt adjustment from -1.0 to 1.0.
 */
void Camera::adjustServoPosition(float normDeltaHorizontal, float normDeltaVertical)	{
					
	/* adjust for the fact that servo overshoots based on image input */
	normDeltaHorizontal /= 8.0;
	normDeltaVertical /= 10.0;
	
	/* compute horizontal goal */
	float currentH = horizontalServo->Get();
	float normCurrentH = RangeToNormalized(currentH, 1);
	float normDestH = normCurrentH + normDeltaHorizontal;	
	/* narrow range keep servo from going too far */
	if (normDestH > 1.0) normDestH = 1.0;
	if (normDestH < -1.0) normDestH = -1.0;			
	/* convert input to servo range */
	float servoH = NormalizeToRange(normDestH);

	/* compute vertical goal */
	float currentV = verticalServo->Get();
	float normCurrentV = RangeToNormalized(currentV, 1);
	float normDestV = normCurrentV + normDeltaVertical;	
	if (normDestV > 1.0) normDestV = 1.0;
	if (normDestV < -1.0) normDestV = -1.0;
	/* convert input to servo range */
	float servoV = NormalizeToRange(normDestV,0.0,1.0 /*0.2, 0.85*/ /*MIN_SERVO_VERTICAL, MAX_SERVO_VERTICAL*/);

	/* make sure the movement isn't too small */
	if ( fabs(currentH-servoH) > servoDeadband ) {
		horizontalServo->Set( servoH );		
		/* save new normalized horizontal position */
		horizontalPosition = RangeToNormalized(servoH, 1);
	}			
	if ( fabs(currentV-servoV) > servoDeadband ) {
		verticalServo->Set( servoV );
		//printf("servo value: %5.4f\n",servoV);
		verticalPosition = RangeToNormalized(servoV, 1);
	}
}
void Camera::myPanForTarget(Servo *panServo, float minPos, float maxPos)
{
	if (m_panEnabled)
	{
		float step= (maxPos-minPos)/SERVO_STEPS*m_panDir;
		if (m_servoPos+step >= maxPos)
		{
			m_servoPos = maxPos;
			m_panDir = -1.0;
		}
		else if (m_servoPos+step <= minPos)
		{
			m_servoPos = minPos;
			m_panDir = 1.0;
		}
		else
			m_servoPos += step;
	}
	// otherwise just leave it at the set value

	panServo->Set(m_servoPos );
}

void Camera::setPanEnabled(bool pan)
{
	m_panEnabled = pan; 
}

void Camera::setFixedServoPos(float pos)
{
	m_servoPos = pos; 
}

void Camera::setGreenPosition(SecondColorPosition pos)
{
	m_greenPosition = pos;
}

float Camera::getDistanceToTarget()
{
	//printf("particle size=%f\n",getParticleSize();
	return 8.09/sqrt(getParticleSize())-1.2;
	
	// To calibrate this:
	// Find getParticleSize for two or more distances, eg. 4ft and 20ft
	// In spreadsheet enter:
	//    psize1     =1/sqrt(psize1)     dist1
	//    psize2     =1/sqrt(psize2)     dist2
	// Make XY graph of last 2 columns
	// Right click on a point and insert trendline, with equation
	// Put values from equation into the above formula.
}

/**
 * Get horizontal position of target 
 * 
 * @return horizontal position of target range -1.0 to 1.0
 */
float Camera::getTargetX()	{
	return horizontalDestination;
}
/**
 * Get vertical position of target 
 * 
 * @return vertical position of target range -1.0 to 1.0
 */
float Camera::getTargetY()	{
	return verticalPosition;
}
/**
 * Is camera locked on target 
 * 
 * @return True if camera is locked on target
 */
bool Camera::lockedOn()	{
	return foundColor;
}
