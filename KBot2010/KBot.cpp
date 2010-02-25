#include "KBot.h"

#include "ManagerDefense.h"
#include "ManagerMidField.h"
#include "ManagerForward.h"

#include "Relay.h"

static float kfWinchSpeed = 1.0;

#define PERIODIC_SPEED	200		// Speed in Hz of main periodic routines 

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
		
		IterativeRobot::SetPeriod(1.0/PERIODIC_SPEED);

		/************************************/
		// Create and initialize input devices

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
		
		// NOTE:  Programmed move calibration is independent of
		// these values and set in StrategyProgrammedMove.cpp
		m_leftEncoder->SetDistancePerPulse(0.01257);
		m_rightEncoder->SetDistancePerPulse(0.01257);
		m_leftEncoder->Start();
		m_rightEncoder->Start();
		m_leftEncoder->Reset();
		m_rightEncoder->Reset();

		// digital inputs
		m_DefenseSwitch = new DigitalInput(DEFENSE_SWITCH);		
		m_MidFieldSwitch = new DigitalInput(MIDFIELD_SWITCH);		
		m_ForwardSwitch = new DigitalInput(FORWARD_SWITCH);		

		m_gateSensor = new DigitalInput(GATE_SENSOR);

		m_ultrasoundNear = new DigitalInput(ULTRA_NEAR);
		m_ultrasoundFar = new DigitalInput(ULTRA_FAR);
		
		m_armRelease = new Solenoid(SOLENOID_SLOT, ARM_RELEASE);
		m_armRetract = new Solenoid(SOLENOID_SLOT, ARM_RETRACT);
		
		/************************************/
		// Create and initialize output devices
		
		m_leftJaguar1 = new CANJaguar(L_WHEEL1_JAG_ID, CANJaguar::kPercentVoltage);
		//m_leftJaguar = new CANJaguar(1, CANJaguar::kSpeed);
		//m_leftJaguar->ConfigEncoderCodesPerRev((UINT16)3600);
		//m_leftJaguar->SetPID(0.1,0.0,0.0);
		m_leftJaguar1->Set(0.0);
		m_leftJaguar2 = new CANJaguar(L_WHEEL2_JAG_ID, CANJaguar::kPercentVoltage);
		m_leftJaguar2->Set(0.0);

		m_rightJaguar1 = new CANJaguar(R_WHEEL1_JAG_ID, CANJaguar::kPercentVoltage);
		//m_rightJaguar = new CANJaguar(2, CANJaguar::kSpeed);
		//m_rightJaguar->ConfigEncoderCodesPerRev((UINT16)360);
		//m_rightJaguar->SetPID(0.1,0.0,0.0);
		m_rightJaguar1->Set(0.0);
		m_rightJaguar2 = new CANJaguar(R_WHEEL2_JAG_ID, CANJaguar::kPercentVoltage);
		m_rightJaguar2->Set(0.0);

		m_robotDrive = new KBotDrive(m_leftJaguar1, m_leftJaguar2, m_rightJaguar1, m_rightJaguar2);
		m_winchMotor = new CANJaguar(WINCH_JAG_ID, CANJaguar::kPercentVoltage);
		m_winchMotor->Set(0.0);
		m_rollerMotor = new CANJaguar(ROLLER_JAG_ID, CANJaguar::kPercentVoltage);
		m_rollerMotor->Set(0.0);
		
		m_driverStation = DriverStation::GetInstance();
		m_priorPacketNumber = 0;
		m_dsPacketsPerSecond = 0;
		
//		m_compressor = new Compressor(PRESSURE_SWITCH_CHANNEL, COMPRESSOR_RELAY_CHANNEL);
		m_compressorRelay = new Relay(COMPRESSOR_RELAY_CHANNEL,Relay::kForwardOnly);
		m_pressureSwitch = new DigitalInput(PRESSURE_SWITCH_CHANNEL);
		
		m_kicker = new Kicker(this, PISTON_ACTUATOR, PISTON_RELEASE, ELECTROMAGNET_CHANNEL);
		// Create high level controllers
		// **** MUST BE AFTER ALL OTHER OBJECTS ARE CREATED, as the constructors
		// create links to the other objects. ****

		m_pCamera = new KbotCamera();
		
		m_teleMacros = new RobotMacros(this);
				
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		m_nStartTime = 0;
		
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
			m_kicker->Init();
		}
		else if (m_MidFieldSwitch->Get())
		{
			m_autoManager = new ManagerMidField(this);
			m_kicker->Init();
		}
		else if (m_ForwardSwitch->Get())
		{
			m_autoManager = new ManagerForward(this);
			m_kicker->Init();
		}
		else	// should do something more clever here
		{
			m_autoManager = new ManagerMidField(this);
			m_kicker->Init();
		}
		
		m_pCamera->init();
		m_compressorRelay->SetDirection(Relay::kForwardOnly);
		
	}
	
	void KBot::DisabledInit(void) {
		m_disabledPeriodicLoops = 0;			// Reset the loop counter for disabled mode

	}
	
	void KBot::AutonomousInit(void) {
		m_autoPeriodicLoops = 0;				// Reset the loop counter for autonomous mode
		m_autoManager->setState(knInitial);
		
		// set gyro initial direction
		m_gyro->Reset();

		m_autoManager->reset();
		m_autoManager->init();
		
		m_kicker->Kick();		// This forces it to try to get the crossbow at the beginning of auto (usually there won't be enough pressure for this to work)
	}
	
	void KBot::TeleopInit(void) {
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
		m_dsPacketsPerSecond = 0;				// Reset the number of dsPackets in current second

		m_gyro->Reset();

		m_compressorRelay->SetDirection(Relay::kForwardOnly);

		m_pDashboardDataSender = new DashboardDataSender();

		//printf("Setting encoders\n");
	}
	
	/********************************** Periodic Routines *************************************/
	
	void KBot::DisabledPeriodic(void)  {
		// Runs at 200 Hz
		// feed the user watchdog at every period when disabled
		GetWatchdog().Feed();
		
		// increment the number of disabled periodic loops completed
		m_disabledPeriodicLoops++;
		
		if ((m_disabledPeriodicLoops % 40) == 0) { // 5 Hz
			// TODO:  any disabled mode periodic bookkeeping
		}
		if ((m_disabledPeriodicLoops % 200) == 0) { // 1 Hz
			//printf("Gate: %d\n",getGateSensorState());
			//printf("Ultrasound Near/Far: %d/%d\n",getNearUltrasoundState(),getFarUltrasoundState());
			//printf("Pressure switch %d\n",m_pressureSwitch->Get());
			
			printf("Encoders: left = %d    right = %d\n",m_leftEncoder->Get(), m_rightEncoder->Get());
		}
	}
	
	/*!
	 * TODO: document
	*/
	void KBot::AutonomousPeriodic(void) {
		// Runs at 200 Hz
		// feed the user watchdog at every period when in autonomous
		GetWatchdog().Feed();
		
		if (m_autoPeriodicLoops == 1000) { // after 5 seconds
			m_kicker->Kick(); // compressor should be up to pressure; so get crossbow (out of way of rollers)
		}
		
		if ((m_autoPeriodicLoops % 200) == 0) { // 1 Hz
			//printf("Auto count=%d\n",m_autoPeriodicLoops);
		}
		
		if ((m_autoPeriodicLoops % 40) == 0) { // 5 Hz

			// TODO:  target acquistion with new camera.  Modify
		}
		
		if ((m_autoPeriodicLoops % 200) == 0) { // 1 Hz
			controlCompressor();
		}
		
		// call the manager to figure out what to to now
		m_autoManager->onClock(true);
		m_kicker->onClock();
		
		m_autoPeriodicLoops++;

		if ((m_autoPeriodicLoops % (int)GetLoopsPerSec()) == 0)
		{
			printf("Auto seconds: %d %d\n", (m_autoPeriodicLoops / (int)GetLoopsPerSec()),(int)GetLoopsPerSec());
		}
	}

	void KBot::controlCompressor(void)
	{
		// control the compressor based on pressure switch reading
		if (0 == m_pressureSwitch->Get())
		{
			m_compressorRelay->SetDirection(Relay::kForwardOnly);
			m_compressorRelay->Set(Relay::kForward);
		}
		else
		{
			m_compressorRelay->SetDirection(Relay::kForwardOnly);
			m_compressorRelay->Set(Relay::kOff);				
		}
	}
	
	void KBot::TeleopPeriodic(void) {
		// Runs at 200 Hz
		// feed the user watchdog at every period when in teleop
		GetWatchdog().Feed();

		if ((m_telePeriodicLoops % 200) == 0) { // 1 Hz
			//printf("Tele count=%d\n",m_telePeriodicLoops);
			//printf("Pressure switch %d     gyro: %f\n",m_pressureSwitch->Get(),m_gyro->GetAngle());
			controlCompressor();
		}

		if ((m_telePeriodicLoops % 40) == 0) { // 5 Hz
			vector<Target> vecTargets = m_pCamera->findTargets();

			if (vecTargets[0].m_score > MINIMUM_SCORE)
			{
				m_pDashboardDataSender->sendVisionData(0.0, m_gyro->GetAngle(), 0.0, vecTargets[0].m_xPos / vecTargets[0].m_xMax, vecTargets);
			}				
		}

		if (getRightStick()->GetTrigger())
		{
			m_kicker->Kick();
		}			
		
		if (getRightStick()->GetRawButton(CAPTURE_BUTTON))
		{
			m_teleMacros->Set(mcCAPTURE);
		}
		else if (getRightStick()->GetRawButton(AIM_BUTTON))
		{
			m_teleMacros->Set(mcAIM);
		}
		else
		{
			m_teleMacros->Set(mcDRIVE);
		}
		
		if (getLeftStick()->GetRawButton(OPERATOR_PISTON_OUT_BUTTON))
		{
			//printf("Get crossbow\n");
			m_kicker->onAction(GET_CROSSBOW);
		}
		else if (getLeftStick()->GetRawButton(OPERATOR_PISTON_IN_BUTTON))
		{
			//printf("Tension crossbow\n");
			m_kicker->onAction(TENSION_CROSSBOW);
		}
		else if (getLeftStick()->GetRawButton(OPERATOR_EM_ON_BUTTON))
		{
			//printf("EM On\n");
			m_kicker->onAction(EM_ON);
		}
		else if (getLeftStick()->GetRawButton(OPERATOR_EM_OFF_BUTTON))
		{
			//printf("KICK!\n");
			m_kicker->onAction(KICK);
		}
		else
		{
			m_kicker->onClock();
		}

		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;

		/*
		 * NOTE:  Anything placed here will be called on each iteration of the periodic loop.
		 * Since the default speed of the loop is 200 Hz, code should only really be placed here
		 * for I/O that can respond at a 200Hz rate. (e.g. the Jaguar speed controllers
		 */
		
		if ((m_telePeriodicLoops % 2) == 0) {
			/* 
			 * Code placed in here will be called on every alternate iteration of the periodic loop.
			 * Assuming the default 200 Hz loop speed, code should only really be placed here for 
			 * I/O that can respond at a 100Hz rate. (e.g. the Victor speed controllers)
			 */
			
			// put 100Hz Victor control here
		}
		
		if ((m_telePeriodicLoops % 4) == 0) {
			/* 
			 * Code placed in here will be called on every fourth iteration of the periodic loop.
			 * Assuming the default 200 Hz loop speed, code should only really be placed here for 
			 * I/O that can respond at a 50Hz rate. (e.g. the Hitec HS-322HD servos)
			 */

			//TODO: this is here so it doesn't saturate the serial bus.  It might be able to move to 200Hz via 2can adapter.  Check if it works at this speed.
			
			// put 50Hz servo control here
			// if t >= 100 s in teleop allow arm/winch operation
			// TODO:  remove test stuff
			if (true) //m_telePeriodicLoops >= 20000)
			{
				if (getLeftStick()->GetTrigger())
				{
					getArmRelease()->Set(true);
					getArmRetract()->Set(false);
				}
				else	// pull arm in by default
				{
					getArmRelease()->Set(false);
					getArmRetract()->Set(true);				
				}
				
				// control winch independently of arm
				if (getLeftStick()->GetRawButton(WINCH_BUTTON))
				{
					getWinchMotor()->Set(kfWinchSpeed);	
				}
				else
				{
					getWinchMotor()->Set(0.0f);
				}
			}
			// this is where the actual robotic driving is done
			m_teleMacros->OnClock();
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
		
		if (m_telePeriodicLoops % 200 == 0) {
			/* 
			 * Code placed in here will be called once a second
			 * 
			 * For this example code, print out the number of seconds spent in current teleop mode,
			 * as well as the number of DS packets received in the prior second.
			 */
			//printf("Teleop seconds: %d\n", (int)(m_telePeriodicLoops / 200));
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

	/* Accessor methods
	 */
	KBotDrive *KBot::getKBotDrive()
	{
		return m_robotDrive;
	}
	SpeedController *KBot::getWinchMotor()
	{
		return m_winchMotor;
	}
	SpeedController *KBot::getRollerMotor()
	{
		return m_rollerMotor;
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
	int KBot::getAutoPattern()
	{
		// Read switches and return pattern to use (0-3)
		return 1;
	}
	int KBot::getAutoDirection()
	{
		return 1;
	}
	START_ROBOT_CLASS(KBot);
