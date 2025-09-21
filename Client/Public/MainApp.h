#pragma once
#include "Client_Defines.h"

class CMainApp
{
protected: // enabler를 freind 키워드 없이 사용하려고 protected를 사용함.
			// protected를 사용함에 있어 전혀 문제가 없지만, 좀 더 엄격한 보안을 위한다면 private + friend를 사용하는게 맞다.
	CMainApp();
	~CMainApp();

public:
	HRESULT Init();
	void Tick(_double dt);
	void Render();
	
	void Test();
	void Test2();

private:

};

