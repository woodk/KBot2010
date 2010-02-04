#include "StrategyCapture.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyCapture::StrategyCapture(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
}

/*
Destructor cleans up
*/
StrategyCapture::~StrategyCapture()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyCapture::apply()
{
    eState nNewState = knCapture;    // assume we will keep running

    // Keep turning until we find a target
    if (TargetCaptured())
    {
        // start tracking the target
        nNewState = knCapture;
    }
    else	// TODO:  capture logic here
    {
    }

    return nNewState;
}

void StrategyCapture::init()
{
    printf("Spin state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
    //m_robotDrive->setTorque(120);
}

/* Check camera to see if we can see any targets */
bool StrategyCapture::TargetCaptured()
{
	// TODO: set up to operate with new camera
	return false;
}
