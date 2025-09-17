#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

using LayerID = _ubyte; //uint8_t; -> 1바이트 -> 8비트 
						// 오브젝트가 속한 레이어 id, 최대 32개 권장(비트마스크가 32비트이기 때문)
						/*
							실제로 8비트의 범위는 0 ~ 255 (2의 8제곱 갯수)이지만,
							AND 연산 충돌 방식을 생각하면 32비트 중에서 1은 딱 하나만 존재해야 함 -> 그래서 레이어가 32개
							그래서 32개를 넘으면 사용하고 있는 충돌 방식과는 맞지 않게 된다.
						*/
using TagID = _uint; // uint32_t; -> 4바이트 -> 32비트  // 문자열 태그를 숫자 id로 치환해 저장할 때 쓰는 id

struct CollisionFilter // 충돌 분류/ 마스크 (박스 2D 스타일)
{
	_uint categoryBits = 1u; // 어떤 카테고리인지. 1u -> 0번째 비트만 1
	_uint maskBits = ~0u;
	/*
		- 어떤 카테고리와 충돌할 것인지.
		~0u -> 모든 비트가 1-> 모든 카테고리와 충돌
			이는 AND 연산자를 사용하기 때문. 모든 비트가 1이면 모두 TRUE이기 때문이다.
		
		단, 실제 충돌 성립은 상대도 나를 허용해야 하므로 상대의 maskBits가 내 categoryBits를 막으면 충돌하지 않는다.

		예)
			bit0: defalut	-> 0001b
			bit1: player	-> 0010b
			bit2: enemy		-> 0100b
			bit3: bullet	-> 1000b
				
			마이		카테고리 bit0, 마스크 ~0u(모두허용)
			몬스터		카테고리 bit2, 마스크 ~0u(모두허용)
			플레이어	카테고리 bit1, 마스크 1110b (default만 제외)

			마이와 몬스터 충돌 검사
				마이	카테고리 0001b , 몬스터		마스크 1111b => 0001b and 1111b => true
				몬스터	카테고리 0100b , 마이		마스크 1111b => 0100b and 1111b => true
				결과는 충돌 성공

			마이와 플레이어 충돌 검사
				마이		카테고리 0001b , 플레이어	마스크 1110b => 0001b and 1110b => false
				플레이어	카테고리 0010b , 마이		마스크 1111b => 0010b and 1111b => true
				결과는 충돌 실패
	*/
};

struct ObjectMeta
{
	LayerID layerID = 0;
	TagID	tagID = 0;

	CollisionFilter filter{};

	_bool	visible = true; // 렌더 등 표시 여부
	_bool	paused = false; // 업데이트 정지
	_double timescale = 1.f; // 레이어/ 개별 시간 스케일
};

struct SvHash
{
	using is_transparent = void;
	size_t operator()(string_view sv) const noexcept
	{
		return std::hash<std::string_view>{}(sv);
	}

	size_t operator()(const string& s) const noexcept
	{
		return (*this)(std::string_view{ s });
	}

	size_t operator()(const char* s) const noexcept
	{
		return (*this)(std::string_view{ s });
	}
};

struct SvEq
{
	using is_transparent = void;
	bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
};
// 위 두 구조체가 이형 lookup

class TagDB
{
public:
	TagID GetOrCreate(std::string_view name) // ID 반환 OR ID 생성 함수
	{
		auto it = m_NameToTagID.find(name);
		if(it != m_NameToTagID.end()) return it->second;

		TagID id = static_cast<TagID>(m_Names.size() + 1); // 사이즈 + 1 만큼 태그id 생성
		m_Names.emplace_back(name);
		m_NameToTagID.emplace(m_Names.back(), id);
		return id;
	}

	TagID Find(std::string_view name) const // 찾아서 ID를 반환
	{
		auto it = m_NameToTagID.find(name);
		if (it == m_NameToTagID.end()) return TagID(0);
		return it->second;
	}

	std::string_view Name(TagID id) const // 이름(문자열) 반환
	{
		if (id == 0 || id > m_Names.size()) return {};
		return m_Names[id - 1];
	}

	/*
		string_view를 사용하는 이유
			- 입력 전용 파라미터 : 읽기만 할 때는 할당 없이 받는다. -> 성능 관점에서 좋음
			- 태그가 이미 존재하는지 조회만 할 때 임시 string 생성/ 할당을 피할 수 있다.
				- 이형 lookup 설정 -> 진짜 무할당이 될 수 있다.
					-> 투명해시, 동등비교
			즉, 성능과 비용적면에서 좋다.
	*/

private:
	unordered_map<string, TagID,SvHash,SvEq> m_NameToTagID; // 문자열에서 ID로 바뀐 것들을 보관
	vector<string> m_Names; // 문자열을 보관
};

END