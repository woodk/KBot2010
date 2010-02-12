#ifndef STRATEGY_TRACK_H
#define STRATEGY_TRACK_H

#include "Strategy.h"

/*
The strategy used to track another robot until
we get close enough to attack.
*/
class StrategyTrack : public Strategy
{
public:

    // Constructor initializes object
	StrategyTrack(KBot* kbot);

    // Destructor cleans up
    ~StrategyTrack();

    // Should return attack until we lose sight of target
    eState apply();
    
    void init();

private:
    
    KbotPID *m_cameraTurnCtrl;
    int m_lossCount;
    float m_xval;
    
    // Check if target robot is close enough to attack
    bool getTargetAttackable();

    // Check if target robot is has escaped
    bool getTargetEscaped();

};

#endif
