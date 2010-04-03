#include "KBot.h"

//#define KBOT_DEBUG

#include "CANJaguar.h"
#include "DashboardDataSender.h"
#include "DigitalInput.h"
#include "DriverStation.h"
#include "Encoder.h"
#include "Gyro.h"
#include "Joystick.h"
#include "KbotPID.h"
#include "KBotDrive.h"
#include "Kicker.h"
#include "ManagerDefense.h"
#include "ManagerMidField.h"
#include "ManagerForward.h"
#include "Relay.h"
#include "RobotManager.h"
#include "Solenoid.h"

#ifdef EMULATOR
#include "EmulatorFactory.h"
#else
#include "RobotFactory.h"
#endif

int nLEDCounter = 0;
bool bState = false;

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
		
#ifdef EMULATOR
		HardwareFactory* pHardwareFactory = new EmulatorFactory();
#else
		HardwareFactory* pHardwareFactory = new RobotFactory();
#endif
		
		IterativeRobot::SetPeriod(1.0/PERIODIC_SPEED);

		/************************************/
		// Create and initialize input devices

		// Joysticks
		m_rightStick = pHardwareFactory->BuildJoystick(1);
		m_leftStick = pHardwareFactory->BuildJoystick(2);
		
		// gyro for direction keeping (TODO: wrap in direction keeper class)
		m_gyro = pHardwareFactory->BuildGyro(GYRO_CHANNEL);
		//m_gyro->SetSensitivity(0.0122); // 0.00540 for LPR530AL 4x output (says 0.333 in datasheet)
				// LPR530AL 1x output: (says 0.000830 in datasheet)
				// ADW22307 & ADW22305: 0.0122 or 0.0125 (says 0.007 in datasheet)
		// wheel encoders
		m_leftEncoder = pHardwareFactory->BuildEncoder(L_ENC_A_CHANNEL,L_ENC_B_CHANNEL,false);
		m_rightEncoder = pHardwareFactory->BuildEncoder(R_ENC_A_CHANNEL,R_ENC_B_CHANNEL,true);

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
		m_DefenseSwitch = pHardwareFactory->BuildDigitalInput(DEFENSE_SWITCH);		
		m_MidFieldSwitch = pHardwareFactory->BuildDigitalInput(MIDFIELD_SWITCH);		
		m_ForwardSwitch = pHardwareFactory->BuildDigitalInput(FORWARD_SWITCH);
		m_Pattern1 =  pHardwareFactory->BuildDigitalInput(PATTERN_1_SWITCH);
		m_Pattern2 =  pHardwareFactory->BuildDigitalInput(PATTERN_2_SWITCH);

		m_gateSensor = pHardwareFactory->BuildDigitalInput(GATE_SENSOR);

		m_ultrasoundNear = pHardwareFactory->BuildDigitalInput(ULTRA_NEAR);
		m_ultrasoundFar = pHardwareFactory->BuildDigitalInput(ULTRA_FAR);
		
		/************************************/
		// Create and initialize output devices
		
		m_leftJaguar1 = pHardwareFactory->BuildCANJaguar(L_WHEEL1_JAG_ID, CANJaguar::kPercentVoltage);
		m_leftJaguar1->Set(0.0);
		m_leftJaguar2 = pHardwareFactory->BuildCANJaguar(L_WHEEL2_JAG_ID, CANJaguar::kPercentVoltage);
		m_leftJaguar2->Set(0.0);

		m_rightJaguar1 = pHardwareFactory->BuildCANJaguar(R_WHEEL1_JAG_ID, CANJaguar::kPercentVoltage);
		m_rightJaguar1->Set(0.0);
		m_rightJaguar2 = pHardwareFactory->BuildCANJaguar(R_WHEEL2_JAG_ID, CANJaguar::kPercentVoltage);
		m_rightJaguar2->Set(0.0);

		m_robotDrive = pHardwareFactory->BuildKBotDrive(m_leftJaguar1, m_leftJaguar2, m_rightJaguar1, m_rightJaguar2);
		
		m_rightSideRollerMotor = pHardwareFactory->BuildCANJaguar(R_SIDE_ROLLER_JAG_ID, CANJaguar::kPercentVoltage);
		m_rightSideRollerMotor->Set(0.0);

		m_leftSideRollerMotor = pHardwareFactory->BuildCANJaguar(L_SIDE_ROLLER_JAG_ID, CANJaguar::kPercentVoltage);
		m_leftSideRollerMotor->Set(0.0);
		
		m_rollerMotor = pHardwareFactory->BuildCANJaguar(ROLLER_JAG_ID, CANJaguar::kPercentVoltage);
		m_rollerMotor->Set(1.0);
		
		m_driverStation = pHardwareFactory->BuildDriverStation();
		m_priorPacketNumber = 0;
		m_dsPacketsPerSecond = 0;
		
		m_compressorRelay = pHardwareFactory->BuildRelay(COMPRESSOR_RELAY_CHANNEL,Relay::kForwardOnly);
		m_pressureSwitch = pHardwareFactory->BuildDigitalInput(PRESSURE_SWITCH_CHANNEL);
		
		m_kicker = pHardwareFactory->BuildKicker(this, PISTON_ACTUATOR, PISTON_RELEASE, ELECTROMAGNET_CHANNEL);
		// Create high level controllers
		// **** MUST BE AFTER ALL OTHER OBJECTS ARE CREATED, as the constructors
		// create links to the other objects. ****

		m_teleMacros = new RobotMacros(this);
				
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		m_nStartTime = 0;
		
		k_modes = (char**)calloc(sizeof(char**),7);
		if (k_modes != NULL) {
			k_modes[0] = "(mid)";
			k_modes[1] = " def ";
			k_modes[2] = " for ";
			k_modes[3] = " mid ";
			k_modes[4] = "(mid)";
			k_modes[5] = "(mid)";
			k_modes[6] = "(mid)";
		  }

		printf("KBot Constructor Completed\n");
	}

	KBot::~KBot() {
		;
	}

	
	/********************************** Init Routines *************************************/

	void KBot::RobotInit(void) {
		// Actions which would be performed once (and only once) upon initialization of the
		// robot would be put here.
		if (!m_DefenseSwitch->Get())
		{
			m_autoManager = new ManagerDefense(this);
		}
		else if (!m_MidFieldSwitch->Get())
		{
			m_autoManager = new ManagerMidField(this);
		}
		else if (!m_ForwardSwitch->Get())
		{
			m_autoManager = new ManagerForward(this);
		}
		else	// should do something more clever here
		{
			m_autoManager = new ManagerMidField(this);
		}

		m_kicker->Init();
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
		
//#define KBOT_DEBUG
//#define KBOT_EM_DEBUG
#ifdef KBOT_DEBUG
		if ((m_disabledPeriodicLoops % 200) == 0) { // 1 Hz
			DriverStationEnhancedIO& dseio = DriverStation::GetInstance()->GetEnhancedIO();

			dseio.GetDigitalConfig(1);
			if (++nLEDCounter%7==0)
			{
				bState = !bState;
			}
			dseio.SetLED(nLEDCounter%7+1,bState);
			//printf("DIO D3: %d\n", dseio.GetDigital(3));

			if ((m_disabledPeriodicLoops%4000) == 0) // print headings once per screen
			{
				printf("Gate USNear USFar PressSw L_Enc R_Enc  Mode Pattern\n");
			}
			printf("  %1d    %1d      %1d      %1d    %5d %5d  %s  %1d\n",getGateSensorState(),
					getNearUltrasoundState(),getFarUltrasoundState(), m_pressureSwitch->Get(),
					m_leftEncoder->Get(), m_rightEncoder->Get(),k_modes[!m_DefenseSwitch->Get()+2*!m_ForwardSwitch->Get()+3*!m_MidFieldSwitch->Get()],getAutoPattern());
		}
		
#endif

		// increment the number of disabled periodic loops completed
		m_disabledPeriodicLoops++;
		
		if ((m_disabledPeriodicLoops % 40) == 0) { // 5 Hz
			// TODO:  any disabled mode periodic bookkeeping
		}
	}
	
	/*!
	 * TODO: document
	*/
	void KBot::AutonomousPeriodic(void) {
		// Runs at 200 Hz
		// feed the user watchdog at every period when in autonomous
		GetWatchdog().Feed();
		
		if (m_autoPeriodicLoops == 10) { // after 0.25 seconds
			printf("Initial kick to get crossbow.");
			m_kicker->Kick(); // compressor should be up to pressure; so get crossbow (out of way of rollers)
		}
		
		if ((m_autoPeriodicLoops % 200) == 0) { // 1 Hz
			//printf("Auto count=%d\n",m_autoPeriodicLoops);
		}
		
		if ((m_autoPeriodicLoops % 40) == 0) { // 5 Hz
			m_rollerMotor->Set(0.75);
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
			int nAutoSeconds = (m_autoPeriodicLoops / (int)GetLoopsPerSec());
			printf("Auto seconds: %d %d\n", nAutoSeconds, (int)GetLoopsPerSec());
#ifdef KBOT_EM_DEBUG
			if (!(nAutoSeconds%5))
			{
				m_kicker->Kick();
			}
#endif
		}
	}

	int KBot::getKbotTime()
	{
		return 	(m_autoPeriodicLoops / (int)GetLoopsPerSec());
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

		if ((m_telePeriodicLoops % 200) == 0) { // 1 Hz
			// anything that needs to happen once a second
		}

		if (getRightStick()->GetTrigger() || getLeftStick()->GetTrigger())
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

			// this is where the actual robotic driving is done
			m_teleMacros->OnClock();
			
			// this should send data back to driver statino
			m_pDashboardDataSender->sendIOPortData();

			DriverStationEnhancedIO& dseio = DriverStation::GetInstance()->GetEnhancedIO();
			dseio.GetDigitalConfig(1);

			if (this->getGateSensorState())
			{
				//dseio.SetLED(1,true);
				dseio.SetLEDs(255);
			}
			else
			{
				//dseio.SetLED(1,false);
				dseio.SetLEDs(0);
			}
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

void KBot::setLeftSideRollerSpeed(float fSpeed)
{
	// TODO:  check the signs
	m_leftSideRollerMotor->Set(fSpeed);
}
	
void KBot::setRightSideRollerSpeed(float fSpeed)
{
	// TODO:  check the signs
	m_rightSideRollerMotor->Set(fSpeed);
}
	
bool KBot::getNearUltrasoundState() 
{
	return m_ultrasoundNear->Get();
}

bool KBot::getFarUltrasoundState() 
{
	return m_ultrasoundFar->Get();
}

bool KBot::getGateSensorState() 
{
	// we have multiple limit switches that will
	// pull to ground when closed, so sensor will return
	// 0 when we have a ball
	return !m_gateSensor->Get();
}
	

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
		
		return (3-(m_Pattern1->Get()*2+m_Pattern2->Get()));
	}
	int KBot::getAutoDirection()
	{
		return 1;
	}
	START_ROBOT_CLASS(KBot);
