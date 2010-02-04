#include "Strategy.h"
#include "KBot.h"

Strategy::Strategy(KBot* kbot) : m_kbot(kbot) 
{
	// initialize any state common to all strategies.
	m_nCurrentState = knInitial;
	m_nPreviousState = knInitial;

	// TODO: These are just guesses--try some different values
	m_nMinimumStateCount = 10;
	m_nMinimumChangeCount = 10;
	
    m_robotDrive = m_kbot->getRobotDrive();	
}
/*!
 * The logic here is:
 * 
 * 1) If in initial state, respond immediately to new state
 * 2) Otherwise, wait at least m_nMinimumStateCount requests
 * before considering state changes.
 * 3) Once we are considering state changes, we need at least
 * m_nMiniumumChangeCount requests for a new state before we
 * change.
 * 
 * WEAKNESSES:  note that we are not currently logging the
 * KIND of new state we are being asked to transition into.
 * This means we could be asked for one particular state a bunch
 * of times and ignore those requests, only to change into a
 * completely different state the next time.  Our strategies
 * only have a few new states, typically, so this should not
 * be a huge big deal.
 */
eState Strategy::execute()
{
	eState nNewState = apply();	// call the concrete strategy

	if (knInitial == m_nCurrentState) // act immediately on init
	{
		m_nCurrentState = nNewState;	// used in logic below on subsequent calls
	}
	else	// apply hysteresis logic
	{
		++m_nCurrentStateCounter;	// count times we have been in this state
		if (m_nCurrentState > m_nMinimumStateCount)
		{
			// we have been in the current state for long enough
			// so start taking new state requests seriously
			if (nNewState != m_nCurrentState)
			{
				++m_nChangeStateCounter;
			}
			else	// state change requests must be contiguous
			{
				m_nChangeStateCounter = 0;
			}
			
			if ((m_nChangeStateCounter > m_nMinimumChangeCount) &&
				(nNewState != m_nPreviousState))
			{
				// passed criteria.  Actually change state.
				m_nPreviousState = m_nCurrentState;
				m_nCurrentState = nNewState;
				m_nChangeStateCounter = 0;
				m_nCurrentStateCounter = 0;
			}
			else if ((m_nChangeStateCounter > 2*m_nMinimumChangeCount) &&
				(nNewState == m_nPreviousState))
			{
				// more severe criteria to swap back to previous state
				m_nPreviousState = m_nCurrentState;
				m_nCurrentState = nNewState;
				m_nChangeStateCounter = 0;
				m_nCurrentStateCounter = 0;
			}
			else	// no criterion fulfilled, do not allow state change
			{
				nNewState = m_nCurrentState;
			}
		}
	}
	
	return nNewState;
}
