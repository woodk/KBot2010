#include "StrategyInactive.h"

/*
Constructor initalizes object
*/
StrategyInactive::StrategyInactive(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
    m_kbot = kbot;
    m_robotDrive = m_kbot->getKBotDrive();
    m_done=false;
}

/*
Destructor cleans up
*/
StrategyInactive::~StrategyInactive()
{
    // Probably does not do anything in particular
}

/*
Once we are inactive, we stay inactive forever
*/
eState StrategyInactive::apply()
{
	m_robotDrive->ArcadeDrive(0.0,0.0);
	if (!m_done)
	{
		printf("Inactive\n");
		m_done=true;
	}
    return knInactive;
}

void StrategyInactive::init()
{
    printf("Inactive state\n");
}
