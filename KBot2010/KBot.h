#ifndef KBOT_H_
#define KBOT_H_

#include <iostream>
#include <cmath>
#include "WPILib.h"
#include <taskLib.h>

#include "Mappings.h"
#include "CANJaguar.h"
#include "KbotCamera.h"
#include "KbotPID.h"
#include "RobotDrive.h"
#include "Kicker.h"

#include "RobotManager.h"
#include "Macros.h"

#include "DashboardDataSender.h"

#include "Solenoid.h"
#include "DigitalInput.h"

class RobotManager;  // circular dependency handling
class RobotMacros;

class KBot : public IterativeRobot
{
public:
	KBot();
	~KBot();
	void RobotInit(void);
	void DisabledInit(void);
	void AutonomousInit(void);
	void TeleopInit(void);
	void DisabledPeriodic(void);
	void AutonomousPeriodic(void);
	void TeleopPeriodic(void);
	void DisabledContinuous(void);
	void AutonomousContinuous(void);
	void TeleopContinuous(void);
	RobotDrive *getRobotDrive();
	SpeedController *getWinchMotor();
	SpeedController *getRollerMotor();
	Solenoid*	getArmRelease() {return m_armRelease;}
	Solenoid*	getArmRetract() {return m_armRetract;}

	Joystick *getLeftStick();
	Joystick *getRightStick();
	Gyro *getGyro();

	Kicker *getKicker() {return m_kicker;}
	DriverStation *getDriverStation();

	RobotManager* getManager() {return m_autoManager;}
	
	bool getNearUltrasoundState() {return m_ultrasoundNear->Get();}
	bool getFarUltrasoundState() {return m_ultrasoundFar->Get();}
	bool getGateSensorState() {return m_gateSensor->Get();}
	
	int getAutoPattern();	// Reads pattern from 2 switches; returns 0-3
	int getAutoDirection();	// ALWAYS RETURNS 1
	int getAutoMode();
	
	// used by strategies
	Encoder* getLeftEncoder() {return m_leftEncoder;}
	Encoder* getRightEncoder() {return m_rightEncoder;}
	KbotCamera* getCamera() {return m_pCamera;}
	
	UINT32 getAutoClocks() {return m_autoPeriodicLoops;}
	
private: 
	// Output devices
	CANJaguar *m_leftJaguar1;
	CANJaguar *m_leftJaguar2;
	CANJaguar *m_rightJaguar1;
	CANJaguar *m_rightJaguar2;

	RobotDrive *m_robotDrive;
	SpeedController *m_winchMotor;
	SpeedController *m_rollerMotor;
	
	// Input devices
	Joystick *m_rightStick;			// joystick 1 (arcade stick 
	Joystick *m_leftStick;			// joystick 2 (operator stick)
	
	Gyro *m_gyro;

	Encoder *m_leftEncoder;
	Encoder *m_rightEncoder;

	DigitalInput *m_DefenseSwitch;
	DigitalInput *m_MidFieldSwitch;
	DigitalInput *m_ForwardSwitch;

	DigitalInput *m_gateSensor;
	
	DigitalInput *m_ultrasoundNear;
	DigitalInput *m_ultrasoundFar;
	
	Solenoid	*m_armRelease;
	Solenoid	*m_armRetract;
	
	Compressor *m_compressor;
	Relay	*m_compressorRelay;
	DigitalInput *m_pressureSwitch;
	
	Kicker *m_kicker;
	
	// High level controllers
	RobotManager *m_autoManager;
	RobotMacros *m_teleMacros;
	
	// camera wrapper class
	KbotCamera* m_pCamera;
	
	// start time
	time_t m_nStartTime;
	
	// Declare a variable to use to access the driver station object
	DriverStation *m_driverStation;
	UINT32 m_priorPacketNumber;		// keep track of the most recent packet number from the DS
	UINT8 m_dsPacketsPerSecond;		// keep track of the ds packets received in the current second
	
	// New dashboard data sender for 2010
	DashboardDataSender *m_pDashboardDataSender;
	
	// Declare variables for each of the eight solenoid outputs
	static const int NUM_SOLENOIDS = 8;
	Solenoid *m_solenoids[(NUM_SOLENOIDS+1)];

	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
	
	// Target position from camera
	float m_targetX; // range 0.0 - 1.0
	float m_targetY; // range 0.0 - 1.0
};

#endif
