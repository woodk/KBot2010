#include "StrategyCircle.h"

/*
Constructor initalizes object
*/
StrategyCircle::StrategyCircle(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_kbot = kbot;
    m_robotDrive = m_kbot->getRobotDrive();
    m_shooter = m_kbot->getShooter();
}

/*
Destructor cleans up
*/
StrategyCircle::~StrategyCircle()
{
    // Probably does not do anything in particular
}

/*
Circle, but not looking for target
*/
eState StrategyCircle::apply()
{
    eState nNewState = knCircle;

    // Just keeps spinning until autonomous is over
    m_robotDrive->ArcadeDrive(-1.0, 1.0, false);
    m_shooter->Drive(0.0, true);
    return nNewState;
}

void StrategyCircle::init()
{
    printf("Circle state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_END_STATE,true);
}
