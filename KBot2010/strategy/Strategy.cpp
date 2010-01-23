#include "Strategy.h"
#include "KBot.h"

Strategy::Strategy(KBot* kbot) : m_kbot(kbot) 
{
	m_camera = m_kbot->getCamera();
}
