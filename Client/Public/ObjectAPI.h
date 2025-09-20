#pragma once
#include "Client_Defines.h"
#include "EngineAPI.h"

namespace Engine::API
{
	/// 메타 빌더 (체이닝)
	struct Meta
	{
		ObjectMeta meta{};
		Meta& Layer(LayerID id) { meta.layerID = id; return *this; }
		Meta& Tag(TagDB& db, string_view name) {
			meta.tagID = db.GetOrCreate(name); return *this;
		}
		Meta& TagID(TagID id) { meta.tagID = id; return *this; }
		Meta& Visible(_bool on) { meta.visible = on; return *this; }
		Meta& Paused(_bool on) { meta.paused = on; return *this; }
		Meta& TimeScale(_double s) { meta.timescale = s; return *this; }
		operator const ObjectMeta&() const{ return meta; }
	}; 

	/// 파라미터 자동 추론 스폰/획득 슈거
	template<class T, class P = monostate>
	inline T* Spawn(IObjectService& svc, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Spawn<T>(meta, p);
	}

	template<class T,class P = monostate>
	inline IGameObject* Clone(IObjectService& svc, string_view key, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Clone<T>(key, meta, p);
	}

	template<class P = monostate>
	inline IGameObject* Acquire(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.Acquire(key, p);
	}

	/// RAII 스타일의 “오브젝트 대여권(핸들)”
	class objHandle
	{
	public:
		objHandle() = default;
		objHandle(IObjectService& svc,string_view key, IGameObject* p)
			:m_svc(&svc),m_key(string(key)),m_ptr(p){} // 대여 시작

		objHandle(const objHandle&) = delete;				// 복사 생성자 삭제
		objHandle& operator=(const objHandle&) = delete;	// 대입 연산자 삭제
		// 이중 반납을 막기 위함

		objHandle(objHandle&& o) noexcept
			:m_svc(o.m_svc),m_key(o.m_key),m_ptr(exchange(o.m_ptr,nullptr)) {}
		// 이동생성. 소유권을 옮기고, 원본 o.m_ptr를 null로 만들어 반납 중복 방지

		objHandle& operator=(objHandle&& o) noexcept
		{
			if (this != &o)
			{
				reset();
				m_svc = o.m_svc;
				m_key = move(o.m_key);
				m_ptr = exchange(o.m_ptr, nullptr);
			}
			return *this;
		}// 이동 대입 : 자기 대입 가드 후, 기존 것을 reset()으로 먼저 반납하고 새 상태로 교체

		~objHandle() { reset(); } // 소멸자에서 자동 반납 호출

		void reset()
		{
			if (m_svc && m_ptr) { m_svc->Release(m_ptr, m_key); m_ptr = nullptr; }
			// 자동 Reales 호출
		}

		IGameObject* get() const { return m_ptr; }					// 원시 포인터 리턴
		IGameObject& operator*() const { return *m_ptr; }			
		IGameObject* operator->() const { return m_ptr; }			// 두 함수는 포인터처럼 사용하기 위함
		explicit operator bool() const { return m_ptr != nullptr; }	// 유효 여부 체크

	private:
		IObjectService* m_svc{}; // 반납을 담당하는 서비스
		string			m_key;	 // 풀/프로토타입 등의 버킷 식별자.
		IGameObject*	m_ptr{}; // 실제 오브젝트 주소. 유효하면 대여중
	};

	/// 풀 획득을 핸들로 바로 받기
	template<class P = monostate>
	inline objHandle AcquireHandle(IObjectService& svc, string_view key, const P& p = {})
	{
		if (auto* g = Acquire(key, p)) return objHandle{ svc,key,g };
		return {};
	}

	/// 스코프 동안 레이어 일시정지/ 가시성 변경
	struct ScopedLayerPause
	{
		IObjectService& svc;
		LayerID id;
		_bool prev{};

		ScopedLayerPause(IObjectService& s, LayerID i, _bool on)
			:svc(s), id(i), prev(s.LayerPaused(i))
		{
			svc.SetLayerPaused(i, on);
		}

		~ScopedLayerPause() { svc.SetLayerPaused(id, prev); }
	};

	struct ScopedLayerVisible
	{
		IObjectService& svc;
		LayerID id;
		_bool prev{};

		ScopedLayerVisible(IObjectService& s, LayerID i, _bool on)
			:svc(s), id(i), prev(s.LayerVisible(i))
		{
			svc.SetLayerVisible(i, on);
		}

		~ScopedLayerVisible() { svc.SetLayerVisible(id, prev); }
	};
	
	//키-기반 프리미티브: 예열/프로토타입 생성도 간결히

	template<class T, class P = monostate>
	inline HRESULT DefineSpawn(IObjectService& svc)
	{
		return svc.DefineSpawn<T,P>();
	}

	template<class T, class P = monostate>
	HRESULT CreateProto(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.CreatePrototype<T, P>(key, p);
	}
}