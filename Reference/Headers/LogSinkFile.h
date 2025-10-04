#pragma once
#include "LogSink.h"

#define MAX_FILE_SIZE 5*1024*1024 // 5MB ũ��

// ���� : ���� �����̼� ��ũ(ũ�� ����)

BEGIN(Engine::log)

// �⺻ 5MB ������ �α� ������ ��ȯ�ϴ� ���� ��ũ
class FileSink : public ISink
{
public:
	FileSink() = default;
	FileSink(string base = "logs/engine", size_t maxbyte = MAX_FILE_SIZE)
		:m_strBase(move(base)), m_byteMax(move(maxbyte))
	{
		
	}
	virtual ~FileSink() = default;

	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		if (!m_offile.is_open()) return; // ������ �����ִ��� üũ, ������ �ʾ����� �Լ� ����.

		m_byte = formatted.size() + 1; // ����Ʈ ���� ���ڷ� ���� ���ڿ� + 1 ��ŭ Ȯ�� -> �� ���ڵ� ����.

		m_offile << formatted << '\n'; // �ؽ�Ʈ ���� �� ���� ���.

		if (m_byte > m_byteMax)
		{

		}
	}

private:
	void RotateOpen()
	{
		if (m_offile.is_open())
			m_offile.close();

		ostringstream strm;

		strm << m_strBase << "" << std::
	}

private:
	string		m_strBase;
	size_t		m_byteMax;
	ofstream	m_offile;
	size_t		m_byte;
	_int		m_index;
};

END