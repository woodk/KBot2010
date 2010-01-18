// Not really sure what this should include.

#ifndef MACROS_H_
#define MACROS_H_

#include "KBot.h"
class KBot;
class Camera;

#include "KbotRobotDrive.h"
#include "Joystick.h"
#include "Pickup.h"


#define CYCLES_TO_BACK_OFF_PICKUP 9
#define CYCLES_TO_SPEED_SHOOTER 30

#define TELE_WEAVE_PERCENT 0.5

// Angle of camera servo when 
#define CAM_MIN 	0.080	// 0.12
#define CAM_MAX 	0.227	// 0.22
#define SHOOT_MIN	0.5		// was 0.4
#define SHOOT_MAX	1.0
#define SHOOT_DEFAULT 0.65	// was 0.5

enum MacroNames
{
    mcNONE=1,
    mcWEAVE=2,
    mcTURNLEFT=4,
    mcTURNRIGHT=8,
    mcDUMP=16
};

class RobotMacros
{
public:
	RobotMacros(KBot *kbot);
	~RobotMacros();
	void Set(MacroNames mac);
	void Clear(MacroNames mac);
	void OnClock();
	
private: 
	KBot *m_kbot;
	MacroNames m_macroState;
	int m_macroCycle;
	
	bool lastTrigger;
	int triggerCycle;
	int reverseShooterCycle;
	bool m_firstTurnCycle;

	float hopperPosition;

	Shooter *m_shooter;
	Pickup *m_pickup;
	Hopper *m_hopper;
	KbotRobotDrive *m_robotDrive;
	Joystick *m_leftStick;
	Joystick *m_rightStick;
	Gyro *m_gyro;
	KbotPID *m_gyroDriveCtrl;
	Camera *m_camera;
	//DriverStation *m_driverStation;
	
	void OperatorControl();
	void OperatorDrive();
	void WeaveDrive();
	void Turn(MacroNames dir);
	void PickupReverse();

};

#endif

