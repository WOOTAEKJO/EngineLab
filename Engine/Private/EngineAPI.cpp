#include "../Public/EngineAPI.h"
#include "../Public/GameInstance.h"

BEGIN(Engine)

struct ServiceRegistry::PImpl //PImpl(pointer to implementation)은 클래스의 구현 세부를 헤더에서 숨기기 위한 C++ 관용구
{
private:
	ManagerContainer* mc = nullptr;

	IDTService& Delta_Time() { return *mc->Get<CDelta_Time>(); }
	IObjectService& Objects() { return *mc->Get<CObjectManager>(); }

	friend class ServiceRegistry;
	friend struct GameInstanceImpl;
};

END

ServiceRegistry::ServiceRegistry()
	:m_impl(make_unique_enabler<PImpl>())
{
}

ServiceRegistry::~ServiceRegistry()
{
}

void ServiceRegistry::_BindInternal(void* mc_cookie)
{
	m_impl->mc = static_cast<ManagerContainer*>(mc_cookie);
}

IDTService& ServiceRegistry::Delta_Time()
{
	return m_impl->Delta_Time();
}

IObjectService& ServiceRegistry::Objects()
{
	return m_impl->Objects();
}
