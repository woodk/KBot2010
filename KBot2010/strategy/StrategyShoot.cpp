#include "StrategyShoot.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyShoot::StrategyShoot(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
}

/*
Destructor cleans up
*/
StrategyShoot::~StrategyShoot()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyShoot::apply()
{
    eState nNewState = knSearch;    // assume we will keep running

   	// TODO: FIRE!
    nNewState = knSearch;

    return nNewState;
}

void StrategyShoot::init()
{
    printf("Shoot state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
    //m_robotDrive->setTorque(120);
}

