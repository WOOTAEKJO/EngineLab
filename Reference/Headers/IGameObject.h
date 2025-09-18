#pragma once
#include "ObjectMeta.h"

BEGIN(Engine)

struct IGameObject
{
protected:
	IGameObject() = default;
	virtual ~IGameObject() = default;

public:
	virtual HRESULT InitPrototype() { return S_OK; }
	// "생성(메모리 확보)”과 “사용 준비”를 분리
	// 풀에서 재활용할 때도 Init()만 다시 호출해 초기화
	// 실패를 bool/HRESULT로 되돌릴 수 있어 ctor에서 예외 던지는 부담을 줄임

	virtual HRESULT InitInstance() { return S_OK; }
	// 클론 전용 초기화 함수

	virtual void ResetInstance() {}
	// 풀 재사용 시 런타임 상태 리셋

	virtual void OnActivate() {}; // 활성화 순간(씬 투입)
	virtual void OnDeActivate() {}; // 비활성화 순간(씬 제거)
};

END