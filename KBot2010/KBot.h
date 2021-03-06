#ifndef KBOT_H_
#define KBOT_H_

#include <iostream>
#include <cmath>

#include "IterativeRobot.h"
#include "Mappings.h"
#include "Macros.h"

class CANJaguar;
class DashboardDataSender;
class DigitalInput;
class Encoder;
class Gyro;
//class KbotCamera;
class KbotPID;
class KBotDrive;
class Kicker;
class ManagerDefense;
class ManagerMidField;
class ManagerForward;
class Relay;
class RobotManager;
class Solenoid;

#define PERIODIC_SPEED	200		// Speed in Hz of main periodic routines 
#define NUM_SOLENOIDS 8

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

	KBotDrive *getKBotDrive();
	SpeedController *getRollerMotor();

	Joystick *getLeftStick();
	Joystick *getRightStick();
	Gyro *getGyro();

	int getKbotTime();
	
	Kicker *getKicker() {return m_kicker;}
	DriverStation *getDriverStation();

	RobotManager* getManager() {return m_autoManager;}
	
	bool getNearUltrasoundState();
	bool getFarUltrasoundState();
	bool getGateSensorState();
	
	int getAutoPattern();	// Reads pattern from 2 switches; returns 0-3
	int getAutoDirection();	// ALWAYS RETURNS 1
	int getAutoMode();
	
	// used by strategies
	Encoder* getLeftEncoder() {return m_leftEncoder;}
	Encoder* getRightEncoder() {return m_rightEncoder;}
	
	void setLeftSideRollerSpeed(float fSpeed);
	void setRightSideRollerSpeed(float fSpeed);
	
	UINT32 getAutoClocks() {return m_autoPeriodicLoops;}
	
private: 
	// Output devices
	CANJaguar *m_leftJaguar1;
	CANJaguar *m_leftJaguar2;
	CANJaguar *m_rightJaguar1;
	CANJaguar *m_rightJaguar2;

	KBotDrive *m_robotDrive;
	SpeedController *m_rollerMotor;

	SpeedController *m_rightSideRollerMotor;
	SpeedController *m_leftSideRollerMotor;

	// Input devices
	Joystick *m_rightStick;			// joystick 1 (arcade stick 
	Joystick *m_leftStick;			// joystick 2 (operator stick)
	
	Gyro *m_gyro;

	Encoder *m_leftEncoder;
	Encoder *m_rightEncoder;

	DigitalInput *m_DefenseSwitch;
	DigitalInput *m_MidFieldSwitch;
	DigitalInput *m_ForwardSwitch;
	DigitalInput *m_Pattern1;
	DigitalInput *m_Pattern2;

	DigitalInput *m_gateSensor;
	
	DigitalInput *m_ultrasoundNear;
	DigitalInput *m_ultrasoundFar;
	
	Solenoid	*m_armRelease;
	Solenoid	*m_armRetract;
	
	Relay	*m_compressorRelay;
	DigitalInput *m_pressureSwitch;
	
	class DashboardDataSender *dds;

	
	
	void controlCompressor();
	
	Kicker *m_kicker;
	
	// High level controllers
	RobotManager *m_autoManager;
	RobotMacros *m_teleMacros;
	
	// start time
	time_t m_nStartTime;
	
	// Declare a variable to use to access the driver station object
	DriverStation *m_driverStation;
	UINT32 m_priorPacketNumber;		// keep track of the most recent packet number from the DS
	UINT8 m_dsPacketsPerSecond;		// keep track of the ds packets received in the current second
	
	// New dashboard data sender for 2010
	DashboardDataSender *m_pDashboardDataSender;
	
	// Declare variables for each of the eight solenoid outputs
	Solenoid *m_solenoids[(NUM_SOLENOIDS+1)];

	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
	
	char **k_modes;
};

#endif
