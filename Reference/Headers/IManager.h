#pragma once
#include <Windows.h>
//#include "Engine_Typedef.h"

BEGIN(Engine)

struct IManager
{

protected:
	virtual ~IManager() = default;

public:
	virtual HRESULT Init() = 0;
	virtual void Shutdown() {};
};

struct ITickable
{
protected:
	virtual ~ITickable() = default;

public:
	virtual void Tick(Engine::_double dt) = 0;
};

END