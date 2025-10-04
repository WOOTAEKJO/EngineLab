#pragma once
#include "Engine_Defines.h"

/*
	ī�װ� ���
	CORE, TIME, JOBS, RES, POOL, HANDLE, GFX, IO, INPUT, AUDIO, PHYS, SCENE, PERF

	�α� �� �� ����
	HH:MM:SS.mmm | F=<frame> | T=<thread> | <LEVEL> | <CATEGORY> | <MESSAGE> | {k=v,...}

	����Ʈ ���� ��Ģ �ּ� �� ��
	���� (Level + Category + Message)�� 1�ʿ� 1ȸ�� ��, �������� suppressed ����

*/
// ���� : ����,ī�װ� ���� + ���ڿ� ��ȯ�� ����

BEGIN(Engine::log)

//����(�ɰ���) ����ü
enum class LEVEL {TRACE_, DEBUGE_, INFO_, WARN_, ERROR_, FATAL_};

//ī�װ� ����ü
enum class CATEGORY {CORE_, TIME_, JOBS_, RES_, POOL_, HANDLE_, GFX_, IO_, INPUT_, AUDIO_, PHYS_,
						SCENE_,PERF_};


//���ڿ� ��ȯ ����
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

	// constexpr : �Լ� ������ ����� ������ �����ϸ�, ��� ���ڷ� ȣ��� �� ����� ������ �ð��� ���ȴ�.
	//				��, ������ �ð��� ���� ������ �� ������ �׷��� �϶�� ��.
}

END