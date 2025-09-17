#pragma once
#include "Manager_Header.h"

BEGIN(Engine)

struct GameInstanceImpl;

class ENGINE_DLL ServiceRegistry
{
private:
	ServiceRegistry();
	~ServiceRegistry();

public:
	ServiceRegistry(const ServiceRegistry&) = delete;
	ServiceRegistry& operator=(const ServiceRegistry&) = delete;

public:
	IDTService& Delta_Time();
	IObjectService& Objects();

private:
	void _BindInternal(void* mc_cookie); // ���� ���δ� -> managercontainer�� void*�� ����

private:
	struct PImpl;
	u_ptr<PImpl> m_impl;

	friend struct GameInstanceImpl;
};

END

/*
*	Interface + ServiceRegistry �Ļ�� ��Ű����
* 
	- ������ �ּ�ȭ: Ŭ���̾�Ʈ�� ����(�Ŵ���, �����̳�, ���ø�)�� ���� �� �� ���/���� �ӵ�/ABI ���� ��
	- ������ API: �ܺηδ� ���� �������̽��� ����. ���� ������ �ٲ� Ŭ���̾�Ʈ �������� �ּ�ȭ.
	- ����/���� ����: ���� ������ ManagerContainer�� ��� �Ŵ����� ����/�ı�/������ ����. Ŭ���̾�Ʈ�� ���񽺸� ��.
	- �׽�Ʈ/��Ƽ �ν��Ͻ� ����: ���� �̱��� ����, �ν��Ͻ���(���� ���Ǻ�)�� ���� ������ ���� ��� ����.

*/