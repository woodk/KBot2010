
#ifndef CANJAGUAR_H
#define CANJAGUAR_H

#include "SpeedController.h"
#include "PIDOutput.h"
#include <semLib.h>
#include <vxWorks.h>

/**
 * Luminary Micro Jaguar Speed Control
 */
class CANJaguar : public SpeedController, public PIDOutput
{
public:
	// The internal PID control loop in the Jaguar runs at 1kHz.
	static const INT32 kControllerRate = 1000;
	static const double kApproxBusVoltage = 12.0;

	typedef enum {kPercentVoltage, kSpeed, kPosition, kCurrent} ControlMode;
	typedef enum {kCurrentFault = 1, kTemperatureFault = 2, kBusVoltageFault = 4} Faults;
	typedef enum {kForwardLimit = 1, kReverseLimit = 2} Limits;
	typedef enum {kPosRef_Encoder = 0, kPosRef_Potentiometer = 1} PositionReference;
	typedef enum {kSpeedRef_Encoder = 0} SpeedReference;
	typedef enum {kNeutralMode_Jumper = 0, kNeutralMode_Brake = 1, kNeutralMode_Coast = 2} NeutralMode;
	typedef enum {kLimitMode_SwitchInputsOnly = 0, kLimitMode_SoftPositionLimits = 1} LimitMode;

	explicit CANJaguar(UINT8 deviceNumber, ControlMode controlMode = kPercentVoltage);
	virtual ~CANJaguar();

	// SpeedController interface
	float Get();
	void Set(float value);

	// PIDOutput interface
	void PIDWrite(float output);

	// Other Accessors
	void SetSpeedReference(SpeedReference reference);
	void SetPositionReference(PositionReference reference);
	PositionReference GetPositionReference(void);
	void SetPID(double p, double i, double d);
	double GetP();
	double GetI();
	double GetD();
	void EnableControl(double encoderInitialPosition = 0.0);
	void DisableControl();
	float GetBusVoltage();
	float GetOutputVoltage();
	float GetOutputCurrent();
	float GetTemperature();
	double GetPosition();
	double GetSpeed();
	bool GetForwardLimitOK();
	bool GetReverseLimitOK();
	UINT16 GetFaults();
	bool GetPowerCycled();
	void SetVoltageRampRate(double rampRate);
	UINT32 GetFirmwareVersion();
	UINT8 GetHardwareVersion();
	void ConfigNeutralMode(NeutralMode mode);
	void ConfigEncoderCodesPerRev(UINT16 codesPerRev);
	void ConfigPotentiometerTurns(UINT16 turns);
	void ConfigSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition);
	void DisableSoftPositionLimits();
	void ConfigMaxOutputVoltage(double voltage);
	void ConfigFaultTime(float faultTime);

protected:
	UINT8 packPercentage(UINT8 *buffer, double value);
	UINT8 packFXP8_8(UINT8 *buffer, double value);
	UINT8 packFXP16_16(UINT8 *buffer, double value);
	UINT8 packINT16(UINT8 *buffer, INT16 value);
	UINT8 packINT32(UINT8 *buffer, INT32 value);
	double unpackPercentage(UINT8 *buffer);
	double unpackFXP8_8(UINT8 *buffer);
	double unpackFXP16_16(UINT8 *buffer);
	INT16 unpackINT16(UINT8 *buffer);
	INT32 unpackINT32(UINT8 *buffer);
	INT32 sendMessage(UINT32 messageID, const UINT8 *data, UINT8 dataSize);
	INT32 receiveMessage(UINT32 *messageID, UINT8 *data, UINT8 *dataSize, float timeout = 0.05);
	void setTransaction(UINT32 messageID, const UINT8 *data, UINT8 dataSize);
	void getTransaction(UINT32 messageID, UINT8 *data, UINT8 *dataSize);
	UINT8 m_deviceNumber;
	ControlMode m_controlMode;
	SEM_ID m_transactionSemaphore;
	double m_maxOutputVoltage;

private:
	void InitJaguar();
};
#endif

