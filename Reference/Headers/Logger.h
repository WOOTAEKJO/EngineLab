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
	mutex				m_mutex; // ������ ����� ����
	vector<ISink*>		m_vecSink;	// ��ũ���� ������ ���� �����̳�
	atomic<uint64_t>	m_frame{ 0 };	// ���� �����尡 �����ϴ� ������ ��ȣ�� �����ϰ� �����ϴ� ����
										// ���� �����尡 ���ÿ� �����ص� ������ �б�/���� ���� �ϰ��� �� ����.
	LEVEL				m_minLevel;		// �ּ� ���� ����


};

END