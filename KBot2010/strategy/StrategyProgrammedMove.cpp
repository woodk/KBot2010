#include "StrategyProgrammedMove.h"

/*
Constructor initalizes object

TODO:  handle new camera/target/kicker hardware
*/
StrategyProgrammedMove::StrategyProgrammedMove(KBot* kbot) : Strategy(kbot)
{
    // Create initial state here
}

/*
Destructor cleans up
*/
StrategyProgrammedMove::~StrategyProgrammedMove()
{
    // Probably does not do anything in particular
}

/*
Spin on the spot
*/
eState StrategyProgrammedMove::apply()
{
	// TODO:  fix this to handle state parameter
    eState nNewState = knProgrammedMove1;    // assume we will keep running

    // Keep turning until we find a target
    if (MoveComplete())
    {
        // assume we want to capture at end of move
        nNewState = knCapture;
    }
    else
    {
    	// TODO:  driving logic here
   		///m_robotDrive->ArcadeDrive(-0.8, -0.9, false);
    }

    return nNewState;
}

void StrategyProgrammedMove::init()
{
    printf("Programmed Move state\n");
	m_kbot->getDriverStation()->SetDigitalOut(DS_TRACK_STATE,true);
}

/* Check camera to see if we can see any targets */
bool StrategyProgrammedMove::MoveComplete()
{
	// TODO: set up to operate with new camera
	return false;
}
