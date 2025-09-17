#include "../Public/GameInstance.h"
#include "../Public/EngineAPI.h"

BEGIN(Engine)

struct GameInstanceImpl //PImpl(pointer to implementation)�� Ŭ������ ���� ���θ� ������� ����� ���� C++ ���뱸
{
	u_ptr<ManagerContainer> managers;
	ServiceRegistry services;
	
	GameInstanceImpl()
		:managers(make_unique_enabler<ManagerContainer>()), services()
	{
		services._BindInternal(managers.get());
	}

};// ���� ������Ʈ��(�Ļ��)

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
