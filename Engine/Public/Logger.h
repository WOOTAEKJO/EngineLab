#pragma once
#include "LogSinkConsole.h"
#include "LogSinkFile.h"
#include "LogRingBuffer.h"

// 역할 : 중앙 로거 - 포멧팅, 레이트 리밋(초당 1회 상세), 프레임 스탬핑, 멀티 싱크 브로드 캐스트

BEGIN(Engine::log)

// 키 값으로 쓰기 위한 데이터 묶음
struct RateKey
{
	bool operator==(const RateKey& o) const
	{
		return (m_lev == o.m_lev) && (m_cat == o.m_cat) && (m_msg == o.m_msg);
	}// 연산자 오버로딩을 통해 키 체크

	LEVEL		m_lev;
	CATEGORY	m_cat;
	string		m_msg;
};

// RateKey 구조체 키를 위한 해쉬 함수
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