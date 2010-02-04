#include "StrategyAim.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyAim::StrategyAim(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
}

/*
Destructor cleans up
*/
StrategyAim::~StrategyAim()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyAim::apply()
{
    eState nNewState = knAim;    // assume we will keep running

    // Keep turning until we find a target
    if (TargetLocked())
    {
        // start tracking the target
        nNewState = knShootHard;  // TODO:  parameterize this
    }
    else	// TODO:  do guidance here
    {
    }

    return nNewState;
}

void StrategyAim::init()
{
    printf("Spin state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_SPIN_STATE,true);
    //m_robotDrive->setTorque(120);
}

/* Check camera to see if we can see any targets */
bool StrategyAim::TargetLocked()
{
	// TODO: set up to operate with new camera
	return false;
}
