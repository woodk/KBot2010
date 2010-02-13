#include "KBot.h"

#include "ManagerDefense.h"
#include "ManagerMidField.h"
#include "ManagerForward.h"

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
		//m_gyro->SetSensitivity(0.0122); // 0.00540 for LPR530AL 4x output (says 0.333 in datasheet)
				// LPR530AL 1x output: (says 0.000830 in datasheet)
				// ADW22307 & ADW22305: 0.0122 or 0.0125 (says 0.007 in datasheet)
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
		m_DefenseSwitch = new DigitalInput(DEFENSE_SWITCH);		
		m_MidFieldSwitch = new DigitalInput(MIDFIELD_SWITCH);		
		m_ForwardSwitch = new DigitalInput(FORWARD_SWITCH);		

		m_IRSensor1 = new DigitalInput(IR_SENSOR_1);

		m_ultrasoundNear = new DigitalInput(ULTRA_NEAR);
		m_ultrasoundFar = new DigitalInput(ULTRA_FAR);
		
		m_armRelease = new Solenoid(SOLENOID_SLOT, ARM_RELEASE);
		
		/************************************/
		// Create and initialize output devices
		
		// TODO:  create Dribbler, Kicker and Arm (others?)
		
		m_leftJaguar = new CANJaguar(1, CANJaguar::kPercentVoltage);
		//m_leftJaguar = new CANJaguar(1, CANJaguar::kSpeed);
		//m_leftJaguar->ConfigEncoderCodesPerRev((UINT16)3600);
		//m_leftJaguar->SetPID(0.1,0.0,0.0);
		m_leftJaguar->Set(0.0);

		m_rightJaguar = new CANJaguar(2, CANJaguar::kPercentVoltage);
		//m_rightJaguar = new CANJaguar(2, CANJaguar::kSpeed);
		//m_rightJaguar->ConfigEncoderCodesPerRev((UINT16)360);
		//m_rightJaguar->SetPID(0.1,0.0,0.0);
		m_rightJaguar->Set(0.0);
		m_robotDrive = new RobotDrive(m_leftJaguar, m_rightJaguar);
		
		m_driverStation = DriverStation::GetInstance();
		m_priorPacketNumber = 0;
		m_dsPacketsPerSecond = 0;
		
		// Create high level controllers
		// **** MUST BE AFTER ALL OTHER OBJECTS ARE CREATED, as the constructors
		// create links to the other objects. ****

		m_pCamera = new KbotCamera();
		
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
		if (m_DefenseSwitch->Get())
		{
			m_autoManager = new ManagerDefense(this);
		}
		else if (m_MidFieldSwitch->Get())
		{
			m_autoManager = new ManagerMidField(this);
		}
		else if (m_ForwardSwitch->Get())
		{
			m_autoManager = new ManagerForward(this);
		}
		else	// should do something more clever here
		{
			m_autoManager = new ManagerMidField(this);
		}
		
		m_pCamera->init();
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
		//m_driverStation->SetDigitalOut(9,false);  only 8 of them!?!
		//m_driverStation->SetDigitalOut(10,false);
		
		// set gyro initial direction
		m_gyro->Reset();
		
		m_autoManager->reset();
		m_autoManager->init();
	}
	
	void KBot::TeleopInit(void) {
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
		m_dsPacketsPerSecond = 0;				// Reset the number of dsPackets in current second

		// TODO:  do we want to reset gyro initial direction?
		m_gyro->Reset();

		m_pDashboardDataSender = new DashboardDataSender();
		
		m_driverStation->SetDigitalOut(4,false);
		m_driverStation->SetDigitalOut(5,false);
		m_driverStation->SetDigitalOut(6,false);
		m_driverStation->SetDigitalOut(7,false);
		m_driverStation->SetDigitalOut(8,false);
		//m_driverStation->SetDigitalOut(9,false); only 8 of them?!?
		//m_driverStation->SetDigitalOut(10,false);

		printf("Setting encoders\n");
		m_leftJaguar->ConfigEncoderCodesPerRev((UINT16)3600);
		m_rightJaguar->ConfigEncoderCodesPerRev((UINT16)360);
		//printf("Setting PID constants\n");
		//m_leftJaguar->SetPID(0.1,0.0,0.0);
		//m_rightJaguar->SetPID(0.1,0.0,0.0);
		//m_leftJaguar->EnableControl();
		//m_rightJaguar->EnableControl();
		//m_leftJaguar->ConfigEncoderCodesPerRev((UINT16)3600);
		//m_rightJaguar->ConfigEncoderCodesPerRev((UINT16)360);
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
	
	/*!
	 * TODO: document
	*/
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

		if ((m_telePeriodicLoops % 10) == 0) { // 20 Hz
			//printf("Signal: L = %f R = %f\n",m_leftJaguar->Get(),m_rightJaguar->Get());
			//printf("Speed: L = %f R = %f\n",(float)(m_leftJaguar->GetSpeed()),(float)(m_rightJaguar->GetSpeed()));
			//printf("Bus voltage = %f\n",m_leftJaguar->GetBusVoltage());

			//printf("Gyro angle = %f\n",m_gyro->GetAngle());			
			//printf("Near state: %d\n",m_ultrasoundNear->Get());
			//printf("Far state: %d\n",m_ultrasoundFar->Get());

			//printf("Right/Left: %lf/%lf\n",m_leftEncoder->GetRate(),m_rightEncoder->GetRate());
			
			vector<Target> vecTargets = m_pCamera->findTargets();
			if (0 != vecTargets.size())
			{
				m_pDashboardDataSender->sendVisionData(0.0, 0.0, 0.0, 1.0, vecTargets);
				
				// TODO: camera drive testing.  Move to appropriate
				// place
				if (getRightStick()->GetTrigger())
				{
					printf("Targets: %d\n",vecTargets.size());
					printf("%lf %lf\n",vecTargets[0].m_xPos,vecTargets[0].m_yPos);
				}			
			}

			if (getRightStick()->GetTrigger())
			{
				m_armRelease->Set(true);
			}
			else
			{
				m_armRelease->Set(false);
			}
			m_driverStation->SetDigitalOut(DS_LED_CAMERA_LOCK, false);
			m_driverStation->SetDigitalOut(DS_LED_IN_RANGE, false);
		}
		
		printf("%d %d %d\n",m_IRSensor1->Get(), m_ultrasoundNear->Get(), m_ultrasoundFar->Get());
		
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
		return 1;
	}
	START_ROBOT_CLASS(KBot);
