#include "../Public/Delta_Time.h"

IDTService::IDTService() = default;
IDTService::~IDTService() = default;

CDelta_Time::CDelta_Time()
{
}

CDelta_Time::~CDelta_Time()
{
}

HRESULT CDelta_Time::Init()
{
	QueryPerformanceFrequency(&m_freq);
	QueryPerformanceCounter(&m_last);

	return S_OK;
}

_double CDelta_Time::Compute_TimeDelta()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	_double dt = _double(now.QuadPart - m_last.QuadPart)
		/ _double(m_freq.QuadPart);

	m_last = now;

	return dt;
}
