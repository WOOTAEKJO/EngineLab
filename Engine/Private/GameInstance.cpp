#include "../Public/GameInstance.h"
#include "../Public/EngineAPI.h"

BEGIN(Engine)

struct GameInstanceImpl //PImpl(pointer to implementation)은 클래스의 구현 세부를 헤더에서 숨기기 위한 C++ 관용구
{
	u_ptr<ManagerContainer> managers;
	ServiceRegistry services;
	
	GameInstanceImpl()
		:managers(make_unique_enabler<ManagerContainer>()), services()
	{
		services._BindInternal(managers.get());
	}

};// 서비스 레지스트리(파사드)

END

CGameInstance::CGameInstance()
{

}

CGameInstance::~CGameInstance()
{

}

HRESULT CGameInstance::Init()
{
	m_impl = make_unique_enabler<GameInstanceImpl>();
	if (!m_impl) return E_FAIL;

	m_impl->managers.get()->Add<CDelta_Time>();
	m_impl->managers.get()->Add<CTimerManager>();
	m_impl->managers.get()->Add<CObjectManager>();

	return S_OK;
}

void CGameInstance::Tick(_double dt)
{
	m_impl->managers->TickAll(dt);
}

ServiceRegistry& CGameInstance::Service()
{
	return m_impl->services;
}

void CGameInstance::Shutdown()
{
	if (m_impl->managers) m_impl->managers->RemoveAll();
}
