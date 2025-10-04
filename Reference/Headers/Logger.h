#pragma once
#include "LogSinkConsole.h"
#include "LogSinkFile.h"
#include "LogRingBuffer.h"

// ���� : �߾� �ΰ� - ������, ����Ʈ ����(�ʴ� 1ȸ ��), ������ ������, ��Ƽ ��ũ ��ε� ĳ��Ʈ

BEGIN(Engine::log)

// Ű ������ ���� ���� ������ ����
struct RateKey
{
	bool operator==(const RateKey& o) const
	{
		return (m_lev == o.m_lev) && (m_cat == o.m_cat) && (m_msg == o.m_msg);
	}// ������ �����ε��� ���� Ű üũ

	LEVEL		m_lev;
	CATEGORY	m_cat;
	string		m_msg;
};

// RateKey ����ü Ű�� ���� �ؽ� �Լ�
struct RateKeyHash
{
	size_t operator()(const RateKey& k) const {
		return std::hash<int>()((int)k.m_lev * 131 + (int)k.m_cat * 17) ^ std::hash<std::string>()(k.m_msg);
	}
};

class Logger
{
protected:
	Logger() = default;
	~Logger() = default;

public:

private:

private:

};

END