#pragma once
#include "Client_Defines.h"

class CMainApp
{
protected: // enabler�� freind Ű���� ���� ����Ϸ��� protected�� �����.
			// protected�� ����Կ� �־� ���� ������ ������, �� �� ������ ������ ���Ѵٸ� private + friend�� ����ϴ°� �´�.
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

