#pragma once
#include "LogSink.h"

// 역할 : 콘솔 출력 싱크

BEGIN(Engine::log)

// 로그를 표준 출력(stdout)에 한 줄씩 찍는 싱크
struct ConsoleSink : public ISink
{
	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		// 레벨(심각도)에 따른 컬러 추가.

		fputs((formatted + "\n").c_str(), stdout);
	} 
};

END