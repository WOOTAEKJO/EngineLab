#pragma once
#include "LogSink.h"

// ���� : �ܼ� ��� ��ũ

BEGIN(Engine::log)

// �α׸� ǥ�� ���(stdout)�� �� �پ� ��� ��ũ
struct ConsoleSink : public ISink
{
	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		// ����(�ɰ���)�� ���� �÷� �߰�.

		fputs((formatted + "\n").c_str(), stdout);
	} 
};

END