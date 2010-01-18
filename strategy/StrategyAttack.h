#ifndef STRATEGY_ATTACK_H
#define STRATEGY_ATTACK_H

#include "Strategy.h"

/*
The strategy used to attack other robots
up-close-and-personal.
*/
class StrategyAttack : public Strategy
{
public:

    // Constructor initializes object
	StrategyAttack(KBot* kbot);

    // Destructor cleans up
    ~StrategyAttack();

    // Should return attack until we lose sight of target
    eState apply();
    
    void init();

private:
    
    Camera *m_camera;
    KbotPID *m_cameraDriveCtrl;
    KbotPID *m_cameraTurnCtrl;
    Shooter *m_shooter;
    Pickup *m_pickup;
    int m_attackCycles;
    int m_lossCount;
    float m_xval,m_yval;

	// Check if target robot is still in view
    bool getTargetInSight();
    
    // Check if target is close enough to attack
    bool getTargetAttackable();

};

#endif
