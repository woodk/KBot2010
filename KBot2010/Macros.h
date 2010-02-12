// Not really sure what this should include.

#ifndef MACROS_H_
#define MACROS_H_

#include "KBot.h"
class KBot; // handle circular includes

#include "RobotDrive.h"
#include "Joystick.h"

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

	// TODO:  add new hardware, rename this class
	// and add more state/strategy-like logic
	RobotDrive *m_robotDrive;
	Joystick *m_leftStick;
	Joystick *m_rightStick;
	Gyro *m_gyro;
	KbotPID *m_gyroDriveCtrl;
	
	void OperatorControl();
	void OperatorDrive();
	void WeaveDrive();
	void Turn(MacroNames dir);
	void PickupReverse();

};

#endif

