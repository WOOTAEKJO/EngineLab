#pragma once
#include "Engine_Defines.h"
#include "EngineAPI.h"

BEGIN(Engine)

template<class T = IGameObject>
class objHandle
{
public:
	objHandle() = default;
	objHandle(IObjectService* svc, T* p, string key)
		:m_svc(svc), m_ptr(p), m_key(move(key)) {} // 대여 시작

	~objHandle() { reset(); } // 소멸자에서 자동 반납 호출

	objHandle(const objHandle&) = delete;				// 복사 생성자 삭제
	objHandle& operator=(const objHandle&) = delete;	// 대입 연산자 삭제
	// 이중 반납을 막기 위함

	objHandle(objHandle&& o) noexcept { swap(o); }

	objHandle& operator=(objHandle&& o) noexcept
	{
		if (this != &o)
		{
			reset();
			swap(o);
		}
		return *this;
	}// 이동 대입 : 자기 대입 가드 후, 기존 것을 reset()으로 먼저 반납하고 새 상태로 교체

	T* release() noexcept {
		T* p = m_ptr;
		m_ptr = nullptr;
		m_svc = nullptr;
		m_key.clear();
		return p;
	}

	void reset()
	{
		if (m_svc && m_ptr) { 
			m_svc->Release(m_ptr, m_key); // 풀 반환
			m_ptr = nullptr;
			m_svc = nullptr;
			m_key.clear();
		}
		// 자동 Reales 호출
	}

	T*			get()			 noexcept { return m_ptr; }					// 원시 포인터 리턴
	const T*	get()			const noexcept { return m_ptr; }
	T& 			operator*()				{ return *m_ptr; }
	const T&	operator*()		const { return *m_ptr; }
	T*			operator->()			{ return m_ptr; }
	const T*	operator->()	const { return m_ptr; }// 두 함수는 포인터처럼 사용하기 위함
	explicit	operator bool() const { return m_ptr != nullptr; }	// 유효 여부 체크

	const string& key() const noexcept { return m_key; }

private:
	void swap(objHandle& o) noexcept
	{
		std::swap(m_svc, o.m_svc);
		std::swap(m_key, o.m_key);
		std::swap(m_ptr, o.m_ptr);
	}

private:
	IObjectService* m_svc = nullptr; // 반납을 담당하는 서비스
	string			m_key;	 // 풀/프로토타입 등의 버킷 식별자.
	T*				m_ptr = nullptr; // 실제 오브젝트 주소. 유효하면 대여중
};

END