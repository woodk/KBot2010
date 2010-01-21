#ifndef KBOT_H_
#define KBOT_H_

#include <iostream>
#include <cmath>
#include "WPILib.h"
#include <taskLib.h>

#include "Mappings.h"
//#include "CurrentSensor.h"
#include "CANJaguar.h"
#include "KbotPID.h"
//#include "KbotVPID.h"
#include "RobotDrive.h"

#include "Shooter.h"
#include "Pickup.h"
#include "Hopper.h"

#include "Camera.h"

#include "RobotManager.h"
#include "Macros.h"

class RobotManager;
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
	Shooter *getShooter();
	Pickup *getPickup();
	Hopper *getHopper();
	Joystick *getLeftStick();
	Joystick *getRightStick();
	Gyro *getGyro();
	Camera *getCamera();
	DriverStation *getDriverStation();
	int getTeamSwitch();
	int getAutoDirection();
	int getAutoMode();
	
private: 
	// Output devices
	Shooter *m_shooter;
	Pickup *m_pickup;
	Hopper *m_hopper;
	Servo *m_left_shifter;
	Servo *m_right_shifter;
	CANJaguar *m_leftJaguar;
	CANJaguar *m_rightJaguar;
	RobotDrive *m_robotDrive;

	// Input devices
	Joystick *m_rightStick;			// joystick 1 (arcade stick 
	Joystick *m_leftStick;			// joystick 2 (operator stick)
	Gyro *m_gyro;
	AnalogChannel *m_accelX_channel;
	AnalogChannel *m_accelY_channel;
	AnalogChannel *m_accelZ_channel;
	Accelerometer *m_accelX;
	Accelerometer *m_accelY;
	Accelerometer *m_accelZ;
	//CurrentSensor *m_leftCurrentSensor;
	//CurrentSensor *m_rightCurrentSensor;
	Encoder *m_leftEncoder;
	Encoder *m_rightEncoder;
	DigitalInput *m_teamSwitch;
	DigitalInput *m_autoDirection;
	DigitalInput *m_autoMode0;
	DigitalInput *m_autoMode1;
			
	Camera *m_camera;

	// High level controllers
	RobotManager *m_autoManager;
	RobotMacros *m_teleMacros;
	
	// Declare a variable to use to access the driver station object
	DriverStation *m_driverStation;
	UINT32 m_priorPacketNumber;		// keep track of the most recent packet number from the DS
	UINT8 m_dsPacketsPerSecond;		// keep track of the ds packets received in the current second
	
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
