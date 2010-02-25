#ifndef HARDWARE_FACTORY_H
#define HARDWARE_FACTORY_H

#include "CANJaguar.h"

#include <vxWorks.h>

class Joystick;

class HardwareFactory
{
public:
	
	HardwareFactory() {;}
	
	virtual ~HardwareFactory() {;}
	
	virtual Joystick* BuildJoystick(UINT32 nNumber) {return 0;}
	
	virtual Gyro* BuildGyro(UINT32 channel) {return 0;}
	
	virtual Encoder* BuildEncoder(UINT32 aChannel, UINT32 bChannel, bool bReverseDirection = false) {return 0;}
	
	virtual DigitalInput* BuildDigitalInput(UINT32 channel) {return 0;}
	
	virtual Solenoid* BuildSolenoid(UINT32 slot, UINT32 channel) {return 0;}
	
	virtual CANJaguar* BuildCANJaguar(UINT8 deviceNumber, CANJaguar::ControlMode controlMode = CANJaguar::kPercentVoltage) {return 0;}
};

#endif
