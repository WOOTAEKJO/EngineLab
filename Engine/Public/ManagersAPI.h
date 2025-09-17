#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

struct ENGINE_DLL IDelta_Time
{
protected:
	IDelta_Time() = default;
	virtual ~IDelta_Time() = default;

public:
	virtual _double Compute_TimeDelta() = 0;
};

struct ENGINE_DLL ITimerManager
{

};


END