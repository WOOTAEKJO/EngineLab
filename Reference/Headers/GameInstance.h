#pragma once
#include "Engine_Defines.h"
/*
	- GameInstace Ŭ������ ��� �Ŵ��� �� �ý����� �帧�� �����ϱ� ������. ��� ���� ������ �ʴ´�.
		��) �Ŵ��� �����̳� ��

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