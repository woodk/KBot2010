#include "KBot.h"

/// TEMPORARY:  for image testing
#include "Vision/AxisCamera2010.h"
#include "Vision/HSLImage.h"
#include "Target.h"

/**
 * This is the K-Bot 2010 main code.
 * 
 * See Mappings.h for joystick button, PWM, digital and analog IO mappings
 * 
 */
	
/**
 * Constructor for this "KBot" Class.
 * 
 * The constructor creates all of the objects used for the different inputs and outputs of
 * the robot.  Essentially, the constructor defines the input/output mapping for the robot,
 * providing named objects for each of the robot interfaces. 
 */
	KBot::KBot(void) {
		printf("KBot Constructor Started\n");

		/************************************/
		// Create and initialize input devices

		// TODO:  create new camera (or use AxisCamera::getInstance)
		// and create other inputs:  BallSensor, ...?
		
		// Joysticks
		m_rightStick = new Joystick(1);
		m_leftStick = new Joystick(2);
		
		// gyro for direction keeping (TODO: wrap in direction keeper class)
		m_gyro = new Gyro(GYRO_CHANNEL);
		// call m_gyro->SetSensitivity here (was 0.0122)
		
		// wheel encoders
		m_leftEncoder = new Encoder(L_ENC_A_CHANNEL,L_ENC_B_CHANNEL,false);
		m_rightEncoder = new Encoder(R_ENC_A_CHANNEL,R_ENC_B_CHANNEL,true);

		// 250 counts per rev.; gear ratio x:y; 6" wheel
		// 1 count = 1/250 * x / y * 2*PI*6 /12 feet
		//         = 0.151  (@ 1:1)
		m_leftEncoder->SetDistancePerPulse(0.01257);
		m_rightEncoder->SetDistancePerPulse(0.01257);
		m_leftEncoder->Start();
		m_rightEncoder->Start();

		// digital inputs
		m_autoDirection = new DigitalInput(AUTO_LEFT_RIGHT);
		m_autoMode0 = new DigitalInput(AUTO_MODE0);
		m_autoMode1 = new DigitalInput(AUTO_MODE1);
		
		/************************************/
		// Create and initialize output devices
		
		// TODO:  create Dribbler, Kicker and Arm (others?)
		
		m_leftJaguar = new CANJaguar(1, CANJaguar::kSpeed);
		m_leftJaguar->SetPIDConstants(1,0.001,100);
		m_leftJaguar->Set(0.0);

		m_rightJaguar = new CANJaguar(2, CANJaguar::kSpeed);
		m_rightJaguar->SetPIDConstants(1,0.001,100);
		m_rightJaguar->Set(0.0);
		m_robotDrive = new RobotDrive(m_leftJaguar, m_rightJaguar);
		
		m_driverStation = DriverStation::GetInstance();
		m_priorPacketNumber = 0;
		m_dsPacketsPerSecond = 0;
		
		// Create high level controllers
		// **** MUST BE AFTER ALL OTHER OBJECTS ARE CREATED, as the constructors
		// create links to the other objects. ****

		m_autoManager = new RobotManager(this);
		m_teleMacros = new RobotMacros(this);
				
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		m_camReset=false;
		printf("KBot Constructor Completed\n");
	}

	KBot::~KBot() {
		;
	}

	
	/********************************** Init Routines *************************************/

	void KBot::RobotInit(void) {
		// Actions which would be performed once (and only once) upon initialization of the
		// robot would be put here.
	}
	
	void KBot::DisabledInit(void) {
		m_disabledPeriodicLoops = 0;			// Reset the loop counter for disabled mode

	}
	
	void KBot::AutonomousInit(void) {
		m_autoPeriodicLoops = 0;				// Reset the loop counter for autonomous mode
		m_autoManager->setState(knInitial);

		m_driverStation->SetDigitalOut(4,false);
		m_driverStation->SetDigitalOut(5,false);
		m_driverStation->SetDigitalOut(6,false);
		m_driverStation->SetDigitalOut(7,false);
		m_driverStation->SetDigitalOut(8,false);

		// TODO:  set gyro initial direction
		
		m_autoManager->reset();
	}
	
	void KBot::TeleopInit(void) {
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
		m_dsPacketsPerSecond = 0;				// Reset the number of dsPackets in current second

		// TODO:  do we want to reset gyro initial direction?
		
		AxisCamera& camera = AxisCamera::getInstance();
		camera.writeResolution(k320x240);
		camera.writeBrightness(0);

		m_pDashboardDataSender = new DashboardDataSender();
		
		m_driverStation->SetDigitalOut(4,false);
		m_driverStation->SetDigitalOut(5,false);
		m_driverStation->SetDigitalOut(6,false);
		m_driverStation->SetDigitalOut(7,false);
		m_driverStation->SetDigitalOut(8,false);

		m_leftJaguar->SetPIDConstants(1.0,0.001,100);
		m_rightJaguar->SetPIDConstants(1.0,0.001,100);
	}
	
	/********************************** Periodic Routines *************************************/
	
	void KBot::DisabledPeriodic(void)  {
		// feed the user watchdog at every period when disabled
		GetWatchdog().Feed();
		checkCameraReset();
		
		// increment the number of disabled periodic loops completed
		m_disabledPeriodicLoops++;
		
		if ((m_disabledPeriodicLoops % 10) == 0) { // 20 Hz
			// TODO:  any disabled mode periodic bookkeeping
		}
		
	}
	
	void KBot::AutonomousPeriodic(void) {
		// feed the user watchdog at every period when in autonomous
		// AUtonomous runs at 50 Hz
		GetWatchdog().Feed();
		checkCameraReset();
		
		if ((m_autoPeriodicLoops % 10) == 0) { // 5 Hz

			// TODO:  target acquistion with new camera.  Modify
			// calls below to reflect actual target situation.
			m_driverStation->SetDigitalOut(DS_LED_CAMERA_LOCK, false);
			m_driverStation->SetDigitalOut(DS_LED_IN_RANGE, false);
		}
		
		// call the manager to figure out what to to now
		m_autoManager->onClock(true);
		
		m_autoPeriodicLoops++;

		if ((m_autoPeriodicLoops % (int)GetLoopsPerSec()) == 0)
		{
			printf("Auto seconds: %d %d\n", (m_autoPeriodicLoops / (int)GetLoopsPerSec()),(int)GetLoopsPerSec());
		}
	}

	
	void KBot::TeleopPeriodic(void) {
		// feed the user watchdog at every period when in teleop
		GetWatchdog().Feed();
		checkCameraReset();

		AxisCamera& camera = AxisCamera::getInstance();
		
		if ((m_telePeriodicLoops % 10) == 0) { // 20 Hz
			// TODO: use new camera to acquire target and
			// change driverstation calls to reflect actual state
			
			if (camera.freshImage()) {
				// get the camera image
				ColorImage *pImage = camera.GetImage();
				
				vector<Target> vecTargets = Target::FindCircularTargets(pImage);
				delete pImage;
				
				m_pDashboardDataSender->sendVisionData(0.0, 0.0, 0.0, 1.0, vecTargets);
			}
			
			m_driverStation->SetDigitalOut(DS_LED_CAMERA_LOCK, false);
			m_driverStation->SetDigitalOut(DS_LED_IN_RANGE, false);
		}
		
		// this is where the actual robotic driving is done
		m_teleMacros->OnClock();
		
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;

		/*
		 * NOTE:  Anything placed here will be called on each iteration of the periodic loop.
		 * Since the default speed of the loop is 200 Hz, code should only really be placed here
		 * for I/O that can respond at a 200Hz rate.  (e.g. the Jaguar speed controllers
		 */
		
		// put 200Hz Jaguar control here
		
		if ((m_telePeriodicLoops % 2) == 0) {
			/* 
			 * Code placed in here will be called on every alternate iteration of the periodic loop.
			 * Assuming the default 200 Hz loop speed, code should only really be placed here for 
			 * I/O that can respond at a 100Hz rate.  (e.g. the Victor speed controllers)
			 */
			
			// put 100Hz Victor control here
		}
		
		if ((m_telePeriodicLoops % 4) == 0) {
			/* 
			 * Code placed in here will be called on every fourth iteration of the periodic loop.
			 * Assuming the default 200 Hz loop speed, code should only really be placed here for 
			 * I/O that can respond at a 50Hz rate.  (e.g. the Hitec HS-322HD servos)
			 */
			
			// put 50Hz Victor control here
		}
		
		if (m_ds->GetPacketNumber() != m_priorPacketNumber) {
			/* 
			 * Code placed in here will be called only when a new packet of information
			 * has been received by the Driver Station.  Any code which needs new information
			 * from the DS should go in here
			 */
			 
			m_priorPacketNumber = m_ds->GetPacketNumber();
			m_dsPacketsPerSecond++;							// increment DS packets received
						
			// TODO:  put Driver Station-dependent code here
			
		}  // if (m_ds->GetPacketNumber()...
		
		if ((m_telePeriodicLoops % (int)GetLoopsPerSec()) == 0) {
			/* 
			 * Code placed in here will be called once a second
			 * 
			 * For this example code, print out the number of seconds spent in current teleop mode,
			 * as well as the number of DS packets received in the prior second.
			 */
			printf("Teleop seconds: %d\n", (m_telePeriodicLoops / (int)GetLoopsPerSec()));
			//printf("  DS Packets: %u\n", m_dsPacketsPerSecond);
			
			// reset the count of DS packets received in the current second
			m_dsPacketsPerSecond = 0;
		}
	} // TeleopPeriodic(void)


/********************************** Continuous Routines *************************************/

	/* 
	 * These routines are not used in this demonstration robot
	 *
	 */
	void KBot::DisabledContinuous(void) {
	}

	void KBot::AutonomousContinuous(void) {
	}

	void KBot::TeleopContinuous(void) {
	}

	void KBot::checkCameraReset(void)
	{
		// TODO:  put camera reset code here if required
	}
	
	/* Accessor methods
	 */
	RobotDrive *KBot::getRobotDrive()
	{
		return m_robotDrive;
	}
	Joystick *KBot::getLeftStick()
	{
		return m_leftStick;
	}
	Joystick *KBot::getRightStick()
	{
		return m_rightStick;
	}
	Gyro *KBot::getGyro()
	{
		return m_gyro;
	}
	DriverStation *KBot::getDriverStation()
	{
		return m_driverStation;
	}
	int KBot::getAutoDirection()
	{
		return m_autoDirection->Get();
	}
	int KBot::getAutoMode()
	{
		return m_autoMode0->Get() + 2*m_autoMode1->Get();
	}
	
	START_ROBOT_CLASS(KBot);
