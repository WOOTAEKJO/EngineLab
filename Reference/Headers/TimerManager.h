#pragma once
#include "Engine_Defines.h"
#include "IManager.h"

BEGIN(Engine)

// --------- Timer ------------

struct Timer
{
	double remainig; // ���� �ð�
	double interval; // �ݺ� �ֱ�
	bool looping; // �ݺ� �Ǵ�

	function<void()> callback; // ������ �Լ� ��ü
};

// --------- TimerService ------------

struct _declspec(novtable) ENGINE_DLL ITimerService
{
protected:
	ITimerService();
	virtual ~ITimerService();

public:
	
};

// --------- TimerManager ------------

class CTimerManager : public IManager , public ITickable, public ITimerService
{
protected:
	CTimerManager() = default;
	~CTimerManager() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_double dt) override;
	
public:
	void Add_Timer();

public:
	virtual void Shutdown() override;

private:
	vector<Timer> m_vecTimers;
};

END

/*
	- �ð� ��� �̺�Ʈ�� �߾ӿ��� ������ �ִ� �ý���
		- ���� ����
			- Ư�� �ð�(Ȥ�� ������) �ڿ� �� ���� �Լ��� ȣ��. ��: `SpawnEnemy()�� 2.0�� �ڿ� ����`.
		- �ֱ��� ����
			- ���� ���ݸ��� �ݺ������� ȣ��. ��: `0.2�ʸ��� Poison ������`.
		- �Ͻ�����/ ���
			- ���� �Ͻ����� �� Ÿ�̸� ���߱�, Ư�� �̺�Ʈ�� �߰��� ��� ����.
		- ��Ȯ�� �ð� ���� ����
			- �� ������ ������ dt�� �ջ��ؼ� Ÿ�̸� ���� ���� üũ.
			- float ���� ���� ���� ���� double�̳� �����е� tick ����� ���⵵ ��.
		- �ݹ�/�Լ� ��ü ����
			- ���� std::function<void()> ���� �� �����ؼ� ���� �� ����.
			- Unity�� Invoke, Unreal�� FTimerManager�� ��ǥ��.
		- �����÷��̿� ����
			- ������ ���� �ڵ塱�� TimerManager::SetTimer(fn, 3.0f) ���� �����θ� ȣ�� �� �ð� ����/���� ���⼺�� ���� �Ŵ����� �ذ�.

*/