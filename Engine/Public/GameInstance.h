#pragma once
#include "Engine_Defines.h"
/*
	- GameInstace 클래스는 모든 매니저 및 시스템의 흐름을 관리하기 때문에. 어느 곳에 속하지 않는다.
		예) 매니저 컨테이너 등

*/

BEGIN(Engine)

class ServiceRegistry;
struct GameInstanceImpl;

class ENGINE_DLL CGameInstance
{
	DECLARE_SHARED_SINGLETON(CGameInstance)
public:

public:
	HRESULT Init();
	void Tick(_double dt);

	ServiceRegistry& Service();

public:
	void Shutdown();

private:
	u_ptr<GameInstanceImpl> m_impl;
};

END