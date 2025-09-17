#include "../Public/TimerManager.h"

ITimerService::ITimerService() = default;
ITimerService::~ITimerService() = default;

HRESULT CTimerManager::Init()
{

	return S_OK;
}

void CTimerManager::Tick(_double dt)
{
}

void CTimerManager::Add_Timer()
{
}

void CTimerManager::Shutdown()
{
	m_vecTimers.clear();
}
