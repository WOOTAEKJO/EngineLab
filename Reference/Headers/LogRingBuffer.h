#pragma once
#include "LogSink.h"

#define MaxMsgNum 2000

// 역할 : 최근 N(기본 2000개)줄 메모리 보관(크래시 직전 로그 확인 용도)

BEGIN(Engine::log)

class RingBufferSink : public ISink
{
public:
	RingBufferSink() = default;
	RingBufferSink(size_t num = MaxMsgNum)
		:m_MsgMaxNum(num)
	{ }
	virtual ~RingBufferSink() = default;

	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		std::lock_guard<std::mutex> lk(m_Mutex); // 뮤텍스 잠금

		if (m_MsgLine.size() == m_MsgMaxNum)
			m_MsgLine.pop_front(); // 메시지 최대 갯수를 넘으면 제일 앞(오래된)에 메시지를 삭제처리

		m_MsgLine.push_back(formatted);
	}

	// 스냅샷 시점의 복사본을 돌려주며, 내부 버퍼는 그대로 유지.
	vector<string> Snapshot()
	{
		lock_guard<mutex>	Ik(m_Mutex); // 뮤텍스 잠금
		return { m_MsgLine.begin(), m_MsgLine.end() }; // 처음부터 끝까지 복사하여 벡터로 반환한다.
	}

private:
	size_t			m_MsgMaxNum;	// 보관할 최대 메시지 수
	deque<string>	m_MsgLine;		// 메시지를 줄 단위로 저장
									// deque -> 양쪽 끝에서 빠르게 넣고 빼기가 가능한 시퀀스
	mutex			m_Mutex;		// 한 번에 하나의 스레드만 들어오게 하기 위한 잠금용 변수
};

END