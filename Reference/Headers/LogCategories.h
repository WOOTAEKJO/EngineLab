#pragma once
#include "Engine_Defines.h"

/*
	카테고리 목록
	CORE, TIME, JOBS, RES, POOL, HANDLE, GFX, IO, INPUT, AUDIO, PHYS, SCENE, PERF

	로그 한 줄 포맷
	HH:MM:SS.mmm | F=<frame> | T=<thread> | <LEVEL> | <CATEGORY> | <MESSAGE> | {k=v,...}

	레이트 리밋 규칙 주석 한 줄
	같은 (Level + Category + Message)는 1초에 1회만 상세, 나머지는 suppressed 집계

*/
// 역할 : 레벨,카테고리 열거 + 문자열 변환만 제공

BEGIN(Engine::log)

//레벨(심각도) 열거체
enum class LEVEL {TRACE_, DEBUGE_, INFO_, WARN_, ERROR_, FATAL_};

//카테고리 열거체
enum class CATEGORY {CORE_, TIME_, JOBS_, RES_, POOL_, HANDLE_, GFX_, IO_, INPUT_, AUDIO_, PHYS_,
						SCENE_,PERF_};


//문자열 변환 제공
constexpr string_view ToStr(LEVEL e)
{
	switch (e)
	{
	case Engine::log::LEVEL::TRACE_:
		return "TRACE";
	case Engine::log::LEVEL::DEBUGE_:
		return "DEBUGE";
	case Engine::log::LEVEL::INFO_:
		return "INFO";
	case Engine::log::LEVEL::WARN_:
		return "WARN";
	case Engine::log::LEVEL::ERROR_:
		return "ERROR";
	case Engine::log::LEVEL::FATAL_:
		return "FATAL";
	default:
		return " ";
	}
}

constexpr string_view ToStr(CATEGORY e)
{
	switch (e)
	{
	case Engine::log::CATEGORY::CORE_:
		return "CORE";
	case Engine::log::CATEGORY::TIME_:
		return "TIME";
	case Engine::log::CATEGORY::JOBS_:
		return "JOBS";
	case Engine::log::CATEGORY::RES_:
		return "RES";
	case Engine::log::CATEGORY::POOL_:
		return "POOL";
	case Engine::log::CATEGORY::HANDLE_:
		return "HANDLE";
	case Engine::log::CATEGORY::GFX_:
		return "GFX";
	case Engine::log::CATEGORY::IO_:
		return "IO";
	case Engine::log::CATEGORY::INPUT_:
		return "INPUT";
	case Engine::log::CATEGORY::AUDIO_:
		return "AUDIO";
	case Engine::log::CATEGORY::PHYS_:
		return "PHYS";
	case Engine::log::CATEGORY::SCENE_:
		return "SCENE";
	case Engine::log::CATEGORY::PERF_:
		return "PERF";
	default:
		return " ";
	}

	// constexpr : 함수 본문이 상수식 제약을 만족하면, 상수 인자로 호출될 때 결과가 컴파일 시간에 계산된다.
	//				즉, 컴파일 시간에 값이 결정될 수 있으면 그렇게 하라는 뜻.
}

END