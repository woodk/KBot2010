#include "KBot.h"

/**
 * This is the K-Bot main code.
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
		
		// Create and initialize input devices
		m_rightStick = new Joystick(1);
		m_leftStick = new Joystick(2);
		m_gyro = new Gyro(GYRO_CHANNEL);
		m_accelX_channel = new AnalogChannel(ACCEL_X_CHANNEL);
		m_accelY_channel = new AnalogChannel(ACCEL_Y_CHANNEL);
		m_accelZ_channel = new AnalogChannel(ACCEL_Z_CHANNEL);
		m_accelX_channel->SetAverageBits(7); // 7 is the default, so this is unnecessary
		m_accelY_channel->SetAverageBits(7);
		m_accelZ_channel->SetAverageBits(7);
		m_accelX = new Accelerometer(m_accelX_channel);
		m_accelY = new Accelerometer(m_accelY_channel);
		m_accelZ = new Accelerometer(m_accelZ_channel);
		//m_leftCurrentSensor = new CurrentSensor(L_CURRENT_CHANNEL);
		//m_rightCurrentSensor = new CurrentSensor(R_CURRENT_CHANNEL);
		m_leftEncoder = new Encoder(L_ENC_A_CHANNEL,L_ENC_B_CHANNEL,false);
		m_rightEncoder = new Encoder(R_ENC_A_CHANNEL,R_ENC_B_CHANNEL,true);
		m_teamSwitch = new DigitalInput(TEAM_SELECT);
		m_autoDirection = new DigitalInput(AUTO_LEFT_RIGHT);
		m_autoMode0 = new DigitalInput(AUTO_MODE0);
		m_autoMode1 = new DigitalInput(AUTO_MODE1);
		
		//m_gyro->SetSensitivity(0.0122);
		m_accelX->SetZero(1.5+0.03*0.3);
		m_accelY->SetZero(1.5-0.04*0.3);
		m_accelZ->SetZero(1.8-0.06*0.3);	// plus 1g gravitation
		m_accelX->SetSensitivity(0.3);
		m_accelY->SetSensitivity(0.3);
		m_accelZ->SetSensitivity(0.3);
		// 250 counts per rev.; gear ratio x:y; 6" wheel
		// 1 count = 1/250 * x / y * 2*PI*6 /12 feet
		//         = 0.151  (@ 1:1)
		m_leftEncoder->SetDistancePerPulse(0.01257);
		m_rightEncoder->SetDistancePerPulse(0.01257);
		m_leftEncoder->Start();
		m_rightEncoder->Start();
		
		
		// Create and initialize output devices
		m_shooter = new Shooter(SHOOTER_CHANNEL);
		m_pickup = new Pickup(PICKUP_CHANNEL);
		m_hopper = new Hopper(HOPPER_CHANNEL);
		m_left_shifter = new Servo(L_SHIFT_CHANNEL);
		m_right_shifter = new Servo(R_SHIFT_CHANNEL);

		m_left_shifter->Set(1.0);
		m_right_shifter->Set(-1.0);
		//m_leftJaguar = new CANJaguar(1, CANJaguar::kCurrent);
		m_leftJaguar = new CANJaguar(1, CANJaguar::kSpeed);
		//m_leftJaguar = new CANJaguar(1, CANJaguar::kPercentVoltage);
		m_leftJaguar->SetPIDConstants(1,0.001,100);
		m_leftJaguar->Set(0.0);
		//m_rightJaguar = new CANJaguar(2, CANJaguar::kCurrent);
		m_rightJaguar = new CANJaguar(2, CANJaguar::kSpeed);
		//m_rightJaguar = new CANJaguar(2, CANJaguar::kPercentVoltage);
		m_rightJaguar->SetPIDConstants(1,0.001,100);
		m_rightJaguar->Set(0.0);
		m_robotDrive = new RobotDrive(m_leftJaguar, m_rightJaguar);
		//m_robotDrive = new RobotDrive(L_WHEEL_CHANNEL, R_WHEEL_CHANNEL);
		//m_robotDrive->SetLeftCurrentSensor(m_leftCurrentSensor);
		//m_robotDrive->SetRightCurrentSensor(m_rightCurrentSensor);
		//m_robotDrive->SetLeftEncoder(m_leftEncoder);
		//m_robotDrive->SetRightEncoder(m_rightEncoder);
		
		//m_leftEncoder->Reset();
		
		m_driverStation = DriverStation::GetInstance();
		m_priorPacketNumber = 0;
		m_dsPacketsPerSecond = 0;
		
		
		// Create high level controllers
		// **** MUST BE AFTER ALL OTHER OBJECTS ARE CREATED, as the constructors
		// create links to the other objects. ****
		m_camera = new Camera(this);
		m_camera->CameraInit();
		if (m_teamSwitch->Get()==0)
		{
			printf("Red team\n");
			m_camera->setGreenPosition(BELOW);
		}
		else
		{
			printf("Blue team\n");
			m_camera->setGreenPosition(ABOVE);
		}
		
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
		m_camera->CameraInit();
	}
	
	void KBot::AutonomousInit(void) {
		m_autoPeriodicLoops = 0;				// Reset the loop counter for autonomous mode
		m_autoManager->setState(knInitial);
		m_camera->CameraInit();
		m_driverStation->SetDigitalOut(4,false);
		m_driverStation->SetDigitalOut(5,false);
		m_driverStation->SetDigitalOut(6,false);
		m_driverStation->SetDigitalOut(7,false);
		m_driverStation->SetDigitalOut(8,false);
		//m_robotDrive->setTorque(MAX_TORQUE);
		if (m_teamSwitch->Get()==0)
		{
			printf("Red team\n");
			m_camera->setGreenPosition(BELOW);
		}
		else
		{
			printf("Blue team\n");
			m_camera->setGreenPosition(ABOVE);
		}
		m_autoManager->reset();
	}
	
	float cur_torque; 
	void KBot::TeleopInit(void) {
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
		m_dsPacketsPerSecond = 0;				// Reset the number of dsPackets in current second
		m_gyro->Reset();
		//cur_torque = MAX_TORQUE;
		m_driverStation->SetDigitalOut(4,false);
		m_driverStation->SetDigitalOut(5,false);
		m_driverStation->SetDigitalOut(6,false);
		m_driverStation->SetDigitalOut(7,false);
		m_driverStation->SetDigitalOut(8,false);
		if (m_teamSwitch->Get()==0)
		{
			printf("Red team\n");
			m_camera->setGreenPosition(BELOW);
		}
		else
		{
			printf("Blue team\n");
			m_camera->setGreenPosition(ABOVE);
		}
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
			m_camera->CameraPeriodic(false);
			m_targetX = m_camera->getTargetX();
			m_targetY = m_camera->getTargetY();
		}
		
		// while disabled, printout the number of seconds of disabled so far
		//if ((m_disabledPeriodicLoops % (int)GetLoopsPerSec()) == 0) {
		//	ShowActivity("Disabled seconds: %d", (m_disabledPeriodicLoops / (int)GetLoopsPerSec()));			
		//}
	}
	
	void KBot::AutonomousPeriodic(void) {
		// feed the user watchdog at every period when in autonomous
		// AUtonomous runs at 50 Hz
		GetWatchdog().Feed();
		checkCameraReset();
		
		if ((m_autoPeriodicLoops % 10) == 0) { // 5 Hz
			m_camera->CameraPeriodic(true);
			m_targetX = m_camera->getTargetX();
			m_targetY = m_camera->getTargetY();
			m_driverStation->SetDigitalOut(DS_LED_CAMERA_LOCK, m_camera->lockedOn());
			float cameraPos = m_camera->getTargetY();
			m_driverStation->SetDigitalOut(DS_LED_IN_RANGE, cameraPos>0.7);
		}
		m_autoManager->onClock(true);
		
		m_autoPeriodicLoops++;
		//m_robotDrive->SetTime(m_autoPeriodicLoops);
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
			m_camera-> CameraPeriodic(true);
			m_targetX = m_camera->getTargetX();
			m_targetY = m_camera->getTargetY();
			m_driverStation->SetDigitalOut(DS_LED_CAMERA_LOCK, m_camera->lockedOn());
	    	float d=m_camera->getDistanceToTarget();
	    	//float realx=m_targetX*d;
	    	//printf("d=%5.4f realx=%5.4f\n",d,realx);
			m_driverStation->SetDigitalOut(DS_LED_IN_RANGE, d<10.0);
			//m_gyro->Reset();
		}
		//m_leftJaguar->Set(-10.0);
		//m_rightJaguar->Set(30.0);

		m_teleMacros->OnClock();
		
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;
		//m_robotDrive->SetTime(m_telePeriodicLoops);
		
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
			if (m_leftStick->GetRawButton(POWER_DUMP_BUTTON))
			{
				m_left_shifter->Set(1.0);
			}
			else
			{
				m_left_shifter->Set(-1.0);
			}
			 
			m_priorPacketNumber = m_ds->GetPacketNumber();
			m_dsPacketsPerSecond++;							// increment DS packets received
						
			// put Driver Station-dependent code here
			//m_robotDrive->SetPulseMode(m_rightStick->GetRawButton(PULSE_BUTTON));
			
			if (false){//m_rightStick->GetRawButton(WEAVE_BUTTON)) {
				m_teleMacros->Set(mcWEAVE);
				
			} else {
				m_teleMacros->Clear(mcWEAVE);
			}
			
			// Enable/Disable Torque control
			//if (m_rightStick->GetRawButton(TORQUE_BUTTON))
			//{
				//m_robotDrive->SetTorqueLimit(false);
				//m_driverStation->ToggleDigitalOut(DS_LED_TORQUE_CTRL);
			//}
			//else
			//{
				//m_robotDrive->SetTorqueLimit(true);
				//m_driverStation->SetDigitalOut(DS_LED_TORQUE_CTRL,false);
			//}
			//if (m_rightStick->GetRawButton(SET_TORQUE_BUTTON))
			//{
				//cur_torque = (m_rightStick->GetZ()+1)*300.0;
				//printf("Torque set to: %f\n",cur_torque);
			//}
			// Low gear mode
			//if (m_rightStick->GetRawButton(HIGH_TORQUE_BUTTON))
			//{
				//m_robotDrive->setTorque(30);
				//m_driverStation->ToggleDigitalOut(DS_LED_TORQUE_CTRL);
			//}
			//else
			//{
				//m_robotDrive->setTorque(145 /*cur_torque MAX_TORQUE*/);
				//m_driverStation->SetDigitalOut(DS_LED_TORQUE_CTRL,false);
			//}
			
			
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
		if (m_leftStick->GetRawButton(RESET_CAMERA_BUTTON))
		{
			if (!m_camReset)
			{
				m_camera->CameraReset();
				printf("Resetting camera\n");
				m_camReset=true;
			}
		}
		else
		{
			m_camReset=false;
		}
	}
	
	/* Accessor methods
	 */
	RobotDrive *KBot::getRobotDrive()
	{
		return m_robotDrive;
	}
	Shooter *KBot::getShooter()
	{
		return m_shooter;
	}
	Pickup *KBot::getPickup()
	{
		return m_pickup;
	}
	Hopper *KBot::getHopper()
	{
		return m_hopper;
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
	Camera *KBot::getCamera()
	{
		return m_camera;
	}
	DriverStation *KBot::getDriverStation()
	{
		return m_driverStation;
	}
	int KBot::getTeamSwitch()
	{
		return m_teamSwitch->Get();
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
