#pragma once
#include "LogSink.h"

#define MAX_FILE_SIZE 5*1024*1024 // 5MB 크기

// 역할 : 파일 로테이션 싱크(크기 기준)

BEGIN(Engine::log)

// 기본 5MB 단위로 로그 파일을 순환하는 파일 싱크
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
		if (!m_offile.is_open()) return; // 파일이 열려있는지 체크, 열리지 않았으면 함수 종료.

		m_byte = formatted.size() + 1; // 바이트 수를 인자로 받은 문자열 + 1 만큼 확보 -> 널 문자도 생각.

		m_offile << formatted << '\n'; // 텍스트 모드로 한 줄을 기록.

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