#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

// --------- AnyParams ------------
struct AnyParams
{
	const void* data = nullptr;				// 실 인자 메모리 (읽기 전용)
	size_t	size = 0;						// 바이트 크기
	type_index type = typeid(void);
}; /*
		타입 소거 : 아무 타입의 값을 "복사/소유 없이" 포인터 + 바이트 길이(+RTTI)로 들고 다니며,
		공통 인터페이스에 건네줄 때 쓰는 읽기 전용 바이트 뭉치이다.
   */

template<class P>
using D = remove_cv_t<remove_reference_t<P>>; // const T, volatile T, const volatile T에서 cv 제거→ 결국 맨살 타입 T

/// 템플릿 타입 P의 레퍼런스/const/volatile 수식을 싹 걷어낸 “순수 타입”의 std::type_index**를 돌려주는 유틸
template<class P>
inline type_index type_of() { return typeid(D<P>); }

struct Key
{
	type_index obj;			// 생성할 "오브젝트 타입"의 typeid(T)
	type_index param;		// obj와 같은 type_index로 변경하면 더 안전함
	//const type_info* param; // 추가 파라미터 타입의 typeid(p) 주소 
	bool operator==(const Key& o) const { return obj == o.obj && param == o.param; }
};/*
	- obj는 std::type_index로 저장 → typeid(T) 비교/해시가 쉬움.
	- param은 파라미터 타입(예: BulletParams)의 type_info* 포인터를 그대로 보관.
		-> 즉, 같은 T라도 P가 다르면 다른 Key가 된다.
  */

struct KeyHash
{
	size_t operator()(const Key& k) const noexcept
	{
		//return hash<type_index>{}(k.obj) ^ (reinterpret_cast<uintptr_t>(k.param) >> 3);
		// 해시: obj의 해시와 param 포인터 값을 조합. >> 3은 하위 3비트를 털어 약간 섞는 트릭.

		size_t h1 = hash<type_index>{}(k.obj);
		size_t h2 = hash<type_index>{}(k.param);
		return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
		// 상수로 hash-combine을 사용.
	}
};

// ------- 오브젝트 매니저에서 사용 -----------
template<class T, class P>
inline Key SpawnKey() { return { typeid(T),type_of<P>() }; }
inline Key SpawnKeyErased(type_index t, type_index p) { return { t,p }; }
// 스폰은 (T,P)

template<class T>
inline Key CreateKey() { return { typeid(T),typeid(monostate) }; }
inline Key CreateKeyErased(type_index t) { return { t,typeid(monostate) }; }
// 생성은 항상 (T,monostate) 키 하나로 통일

// AnyParams로 타입/주소/크기를 함께 포장한다.
inline AnyParams pack(const std::monostate) { return { nullptr,0,typeid(monostate) }; } // 빈통 포장

template<class P>
inline AnyParams pack(const P& p)
{
	return { addressof(p),sizeof(D<P>),typeid(D<P>) };
}
// 타입 P에 대한 포장


END