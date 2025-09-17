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
	void _BindInternal(void* mc_cookie); // 내부 바인더 -> managercontainer를 void*로 주입

private:
	struct PImpl;
	u_ptr<PImpl> m_impl;

	friend struct GameInstanceImpl;
};

END

/*
*	Interface + ServiceRegistry 파사드 아키텍쳐
* 
	- 의존성 최소화: 클라이언트는 구현(매니저, 컨테이너, 템플릿)을 전혀 모름 → 헤더/빌드 속도/ABI 문제 ↓
	- 안정적 API: 외부로는 작은 인터페이스만 보임. 내부 구현이 바뀌어도 클라이언트 재컴파일 최소화.
	- 수명/순서 제어: 엔진 내부의 ManagerContainer가 모든 매니저의 생성/파괴/순서를 통제. 클라이언트는 서비스만 씀.
	- 테스트/멀티 인스턴스 용이: 전역 싱글톤 없이, 인스턴스별(게임 세션별)로 서비스 묶음을 독립 운용 가능.

*/