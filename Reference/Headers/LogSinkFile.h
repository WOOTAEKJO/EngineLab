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
	explicit FileSink(string base = "logs/engine", size_t maxbyte = MAX_FILE_SIZE)
		:m_strBase(move(base)), m_byteMax(maxbyte)
	{
		RotateOpen(); // ���� ����.
	}// explicit -> "logs/engine" ���� ���ڿ� ���ͷ��� �Ͻ������� FileSink�� ��ȯ�Ǵ� �� ����
	virtual ~FileSink() = default;

	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		if (!m_offile.is_open()) return; // ������ �����ִ��� üũ, ������ �ʾ����� �Լ� ����.

		m_byte += formatted.size() + 1; // ����Ʈ ���� ���ڷ� ���� ���ڿ� + 1 ��ŭ Ȯ�� -> �� ���ڵ� ����.
										// ���ڿ����� ��� ������ ����.

		m_offile << formatted << '\n'; // �ؽ�Ʈ ���� �� ���� ���.

		if (m_byte > m_byteMax)
			RotateOpen();
		// ���� ����Ʈ�� �ִ� ����Ʈ�� �Ѿ��ٸ� �� ���� ����.
	}

private:
	void RotateOpen()
	{
		if (m_offile.is_open())
			m_offile.close(); // ������ ���� ������ ����

		ostringstream strm; // ���ϸ��� �����ϱ� ���� ���ڿ� ��Ʈ�� �غ�

		strm << m_strBase << "" << setw(4) << setfill('0') << m_index++
			<< ".log"; // ���ϸ� ����
						// base_ + _ + (���� index_�� 4�ڸ� 0�е�����) + .log

		m_offile.open(strm.str(), std::ios::out | std::ios::trunc);
		// �ش� ��η� ���� ���� ����.
		// trunc -> ������ ������ ���� �ʱ�ȭ, ������ ����

		m_byte = 0; // ������� ����� ����Ʈ �� ī���͸� 0���� ����
	}

private:
	string		m_strBase;	// ���̽� ���ڿ� ����
	size_t		m_byteMax;	// �ִ� ����Ʈ �� ����
	ofstream	m_offile;	// ���Ϸ� �������� ���� �뵵
	size_t		m_byte;		// ���� ����Ʈ ��
	_int		m_index;	// ���� �ε���
};

END