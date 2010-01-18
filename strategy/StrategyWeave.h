#ifndef STRATEGY_WEAVE_H
#define STRATEGY_WEAVE_H

#include "Strategy.h"
#include "KbotRobotDrive.h"

#define AUTO_WEAVE_PERCENT 0.5

/*
The strategy used to weave while looking for a target 
*/
class StrategyWeave : public Strategy
{
public:

    // Constructor initializes object
	StrategyWeave(KBot* kbot);

    // Destructor cleans up
    ~StrategyWeave();

    // Should return attack until we lose sight of target
    eState apply();
    
    void init();

    // Check if we should keep running
    bool getKeepMoving();

private:

    // the time we should run for in seconds
    float m_fRunTimeInSeconds;

    // The number of times this strategy has been called
    int m_nCallCount;
    
    KbotRobotDrive *m_robotDrive;
	Gyro *m_gyro;
	KbotPID *m_gyroDriveCtrl;

};

#endif
