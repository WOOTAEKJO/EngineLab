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
	void AddSink(ISink* sink);
	void SetMinLevel(LEVEL lev);
	void SetFrame(uint64_t frame);
	uint64_t Frame() const;

	void Write(const LEVEL& lev, const CATEGORY& cat, const string& msg, Fileds filed = Fileds{});

public:
	void FrameStart(uint64_t frame);
	void FrameEnd(uint64_t frame, _double cpuMs);

private:

private:
	struct RateState
	{

	};

	unordered_map<RateKey, RateState, RateKeyHash>	m_mapRate;
	mutex				m_mutex; // 스레드 잠금을 위함
	vector<ISink*>		m_vecSink;	// 싱크들을 저장할 벡터 컨테이너
	atomic<uint64_t>	m_frame{ 0 };	// 여러 스레드가 공유하는 프레임 번호를 안전하게 보관하는 변수
										// 여러 스레드가 동시에 접근해도 찢어진 읽기/쓰기 없이 일관된 값 보장.
	LEVEL				m_minLevel;		// 최소 레벨 변수


};

END