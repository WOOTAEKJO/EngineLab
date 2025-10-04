#pragma once
#include "LogCategories.h"

// 역할 : 싱크 인터페이스와 공통 이벤트 구조

BEGIN(Engine::log)

// 구조화된 부가 정보 -> 기계가 집계/검색하기 쉬운 데이터
struct Fileds
{
	// 다른 타입을 값으로 받는 경우
	template <typename T>
	void Add(string Key,const T& value)
	{
		m_kv.emplace_back(move(Key), move(to_string(value)));
	}

	// 문자열을 값으로 받는 경우
	void Add(string key, string value)
	{
		//m_kv.push_back({ key,value }); -> pair를 임시 객체로 만들어 비용이 더 들어감. 또한 인자 값을 복사하는 비용이 들어감
		m_kv.emplace_back(move(key), move(value)); 
		// -> 임시 객체를 만들지 않고 바로 벡터 메모리 안에서 직접 생성. move 함수로 소유권을 이동하여 복사 비용이 없음.
		// 즉, 비용적 면에서 효율이 좋다.

	}

	// 벡터 컨테이너 안에  있는 모든 문자열들을 중괄호 안에 넣어 반환.
	string Get_Str()
	{
		if (m_kv.empty()) return "{}";

		string str = "{ ";

		int iSize = m_kv.size();

		for (int i = 0; i < iSize; i++)
		{
			str += m_kv[i].first;
			str += " = ";
			str += m_kv[i].second;

			if (i + 1 < iSize)
				str += ", ";
		}

		str += " }";
		return str;
	}

private:
	vector<pair<string, string>>	m_kv;
};

// 한 줄 로그의 완성된 정보 묶음
struct Event
{
	LEVEL								level;		//열거체 레벨
	CATEGORY							cat;		//열거체 카테고리
	string								msg;		//메시지 저장 용도
	Fileds								fileds;		// 구조체 Fileds
	uint32_t							threadID;	// 쓰레드 아이디 저장 -> 4바이트
	uint64_t							frame;		// 프레임 저장 -> 8바이트 -> 프레임 시작/끝과 연계
	chrono::system_clock::time_point	wall;		// 현재 시각을 표시하기 위한 변수 -> 사건의 실제 시각
};

// 로그 한 건을 어디로, 어떻게 출력할지를 결정하는 출력 플러그인 인터페이스
struct ISink
{
protected:
	virtual ~ISink() = default;

public:
	virtual void OnMessage(const Event& e, const string& formatted) = 0;
};

END