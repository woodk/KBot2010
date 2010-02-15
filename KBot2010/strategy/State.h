#ifndef STATE_H
#define STATE_H

enum eState
{
    knAim,
    knBackUp,
    knCapture,
    knCircle,
    knProgrammedMove1,
    knProgrammedMove2,
    knProgrammedMove3,
    knSearch,
    knShoot,
    knInactive,
    knInitial,	// ADD NEW STATES ABOVE THIS ONE
    
    // LEGACY STATES:  DO NOT CHANGE BELOW THIS LINE
    knGoStraight,
    knSCurve,
    knCurveLeft,
    knCurveRight,
    knTurn45,
    knTurn90,
    knGoAlongEnd,
    knGoAlongEdge,
    knWeave,
    knSpin,
    knTrack,
    knAttack,
    knLoadBalls,
};

#endif
