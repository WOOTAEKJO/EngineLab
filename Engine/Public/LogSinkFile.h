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
	explicit FileSink(string base = "logs/engine", size_t maxbyte = MAX_FILE_SIZE)
		:m_strBase(move(base)), m_byteMax(maxbyte)
	{
		RotateOpen(); // 파일 생성.
	}// explicit -> "logs/engine" 같은 문자열 리터럴이 암시적으로 FileSink로 변환되는 걸 방지
	virtual ~FileSink() = default;

	virtual void OnMessage(const Event& e, const string& formatted) override
	{
		if (!m_offile.is_open()) return; // 파일이 열려있는지 체크, 열리지 않았으면 함수 종료.

		m_byte += formatted.size() + 1; // 바이트 수를 인자로 받은 문자열 + 1 만큼 확보 -> 널 문자도 생각.
										// 문자열들이 계속 들어오니 누적.

		m_offile << formatted << '\n'; // 텍스트 모드로 한 줄을 기록.

		if (m_byte > m_byteMax)
			RotateOpen();
		// 현재 바이트가 최대 바이트를 넘었다면 새 파일 생성.
	}

private:
	void RotateOpen()
	{
		if (m_offile.is_open())
			m_offile.close(); // 파일이 열려 있으면 닫음

		ostringstream strm; // 파일명을 조립하기 위한 문자열 스트림 준비

		strm << m_strBase << "" << setw(4) << setfill('0') << m_index++
			<< ".log"; // 파일명 생성
						// base_ + _ + (현재 index_를 4자리 0패딩으로) + .log

		m_offile.open(strm.str(), std::ios::out | std::ios::trunc);
		// 해당 결로로 쓰기 모드로 연다.
		// trunc -> 파일이 있으면 내용 초기화, 없으면 생성

		m_byte = 0; // 현재까지 기록한 바이트 수 카운터를 0으로 리셋
	}

private:
	string		m_strBase;	// 베이스 문자열 저장
	size_t		m_byteMax;	// 최대 바이트 수 저장
	ofstream	m_offile;	// 파일로 내보내기 위한 용도
	size_t		m_byte;		// 현재 바이트 수
	_int		m_index;	// 현재 인덱스
};

END