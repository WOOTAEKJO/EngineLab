#pragma once
#include "LogSink.h"

#define MaxMsgNum 2000

// ���� : �ֱ� N(�⺻ 2000��)�� �޸� ����(ũ���� ���� �α� Ȯ�� �뵵)

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
		std::lock_guard<std::mutex> lk(m_Mutex); // ���ؽ� ���

		if (m_MsgLine.size() == m_MsgMaxNum)
			m_MsgLine.pop_front(); // �޽��� �ִ� ������ ������ ���� ��(������)�� �޽����� ����ó��

		m_MsgLine.push_back(formatted);
	}

	// ������ ������ ���纻�� �����ָ�, ���� ���۴� �״�� ����.
	vector<string> Snapshot()
	{
		lock_guard<mutex>	Ik(m_Mutex); // ���ؽ� ���
		return { m_MsgLine.begin(), m_MsgLine.end() }; // ó������ ������ �����Ͽ� ���ͷ� ��ȯ�Ѵ�.
	}

private:
	size_t			m_MsgMaxNum;	// ������ �ִ� �޽��� ��
	deque<string>	m_MsgLine;		// �޽����� �� ������ ����
									// deque -> ���� ������ ������ �ְ� ���Ⱑ ������ ������
	mutex			m_Mutex;		// �� ���� �ϳ��� �����常 ������ �ϱ� ���� ��ݿ� ����
};

END