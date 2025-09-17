#pragma once
#include "Engine_Defines.h"
#include "IManager.h"

BEGIN(Engine)

// --------- DTService ------------

struct _declspec(novtable) ENGINE_DLL IDTService
{
protected:
	IDTService();
	virtual ~IDTService();

public:
	virtual _double Compute_TimeDelta() = 0;
};

// --------- Delta_Time ------------

class CDelta_Time : public IManager, public IDTService
{
protected:
	CDelta_Time();
	~CDelta_Time();

public:
	HRESULT Init();
	_double Compute_TimeDelta();

private:
	LARGE_INTEGER	m_freq = { 0 };
	LARGE_INTEGER	m_last = { 0 };

};

END