#pragma once
#include "Engine_Defines.h"
#include "IManager.h"

BEGIN(Engine)

// --------- Timer ------------

struct Timer
{
	double remainig; // 남은 시간
	double interval; // 반복 주기
	bool looping; // 반복 판단

	function<void()> callback; // 실행할 함수 객체
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
	- 시간 기반 이벤트를 중앙에서 관리해 주는 시스템
		- 지연 실행
			- 특정 시간(혹은 프레임) 뒤에 한 번만 함수를 호출. 예: `SpawnEnemy()를 2.0초 뒤에 실행`.
		- 주기적 실행
			- 일정 간격마다 반복적으로 호출. 예: `0.2초마다 Poison 데미지`.
		- 일시정지/ 취소
			- 게임 일시정지 시 타이머 멈추기, 특정 이벤트는 중간에 취소 가능.
		- 정확한 시간 누적 관리
			- 매 프레임 들어오는 dt를 합산해서 타이머 만료 조건 체크.
			- float 누적 오차 방지 위해 double이나 고정밀도 tick 기반을 쓰기도 함.
		- 콜백/함수 객체 관리
			- 보통 std::function<void()> 같은 걸 저장해서 만료 시 실행.
			- Unity의 Invoke, Unreal의 FTimerManager가 대표적.
		- 게임플레이와 독립
			- “게임 로직 코드”는 TimerManager::SetTimer(fn, 3.0f) 같은 식으로만 호출 → 시간 누적/관리 복잡성은 전부 매니저가 해결.

*/