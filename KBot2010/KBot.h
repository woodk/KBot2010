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

#include "RobotManager.h"
#include "Macros.h"

#include "DashboardDataSender.h"

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

	Joystick *getLeftStick();
	Joystick *getRightStick();
	Gyro *getGyro();

	DriverStation *getDriverStation();

	int getAutoDirection();	// ALWAYS RETURNS 1
	int getAutoMode();
	
	// used by strategies
	Encoder* getLeftEncoder() {return m_leftEncoder;}
	Encoder* getRightEncoder() {return m_rightEncoder;}
	KbotCamera* getCamera() {return m_pCamera;}
	
	UINT32 getAutoClocks() {return m_autoPeriodicLoops;}

	CANJaguar *m_leftJaguar;
	CANJaguar *m_rightJaguar;
	
private: 
	// Output devices

	RobotDrive *m_robotDrive;

	// Input devices
	Joystick *m_rightStick;			// joystick 1 (arcade stick 
	Joystick *m_leftStick;			// joystick 2 (operator stick)
	Gyro *m_gyro;

	Encoder *m_leftEncoder;
	Encoder *m_rightEncoder;

	DigitalInput *m_DefenseSwitch;
	DigitalInput *m_MidFieldSwitch;
	DigitalInput *m_ForwardSwitch;

	DigitalInput *m_IRSensor1;
	
	DigitalInput *m_ultrasoundNear;
	DigitalInput *m_ultrasoundFar;
	
	// High level controllers
	RobotManager *m_autoManager;
	RobotMacros *m_teleMacros;
	
	// camera wrapper class
	KbotCamera* m_pCamera;
	
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
	
	bool m_camReset;
	void checkCameraReset(void);
	
};

#endif
