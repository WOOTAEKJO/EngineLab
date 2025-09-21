#pragma once
#include "ObjHandle.h"

BEGIN(Engine)

enum : LayerID
{
	Defalut = 0, Player = 1, Enemy = 2, Bullet = 3
};

END

namespace Engine::API
{
	/// 메타 빌더(체이닝)
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

	/// 타입 등록만 (파라미터 없음)
	template<class T>
	inline HRESULT Define(IObjectService& svc) { return svc.DefineSpawn<T>(); } 

	/// 프로토타입 생성 (T,P) 자동 썽크 등록
	template<class T, class P = monostate>
	inline HRESULT	CreateProto(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.CreatePrototype<T, P>(key, p);
	}

	/// 프로토타입 기반 풀 예열
	inline HRESULT Prime(IObjectService& svc, string_view key, size_t cnt)
	{
		return svc.PrimePoolProto(key, cnt);
	}

	/// 한 번에 -> 등록 + 프로토타입 생성 + 예열(선택)
	template<class T, class P = monostate>
	HRESULT RegisterType(IObjectService& svc, string_view key, const P& p = {}, size_t cnt = 0)
	{
		HRESULT ok = S_OK;
		ok = svc.DefineSpawn<T>();
		if (FAILED(ok)) return E_FAIL;
		ok = svc.CreatePrototype<T, P>(key, p);
		if (FAILED(ok)) return E_FAIL;
		if (cnt) return svc.PrimePoolProto(key, cnt);
		return S_OK;
	}

	/// 타입 직스폰 여기서 (T,P) 자동 썽크 등록
	template<class T, class P = monostate>
	inline T* Spawn(IObjectService& svc, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Spawn<T,P>(meta, p);
	}

	/// 레이어만 지정하는 직스폰
	template<class T, class P = monostate>
	inline T* SpawnOn(IObjectService& svc, LayerID id, const P& p = {})
	{
		Meta m = API::Meta().Layer(id);
		return svc.Spawn<T, P>(m, p);
	}

	/// 프로토타입 클론
	template<class P = monostate>
	inline IGameObject* Clone(IObjectService& svc, string_view key, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Clone<P>(key, meta, p);
	}

	/// 풀에서 가져옴. 없으면 클론 폴백
	template<class P = monostate>
	inline IGameObject* Acquire(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.Acquire<P>(key, p);
	}

	/// 해제(풀로 반환)
	template<class T = IGameObject>
	inline void Release(IObjectService& svc, T* obj, string_view key)
	{
		svc.Release(obj, string(key));
	}

#pragma region RAII 핸들 버전

	/// 풀 경로에서 핸들로 받기(스코프 종료 시 자동 Release)
	template<class T = IGameObject, class P = monostate>
	inline objHandle<T> AcquireH(IObjectService& svc, string_view key, const P& p = {})
	{
		auto* obj = svc.Acquire<P>(key, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), std::string(key));
	}

	/// 클론 경로에서 핸들로 받기(스코프 종료 시 자동 Release)
	template<class T = IGameObject, class P = monostate>
	inline objHandle<T> CloneH(IObjectService& svc, string_view key, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		auto* obj = svc.Clone<P>(key, meta, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), string(key));
	}

	/// 타입 직스폰 핸들 (풀키 정책을 함께 줄 수 있다.)
	template<class T, class P = monostate>
	inline objHandle<T> SpawnH(IObjectService& svc, const ObjectMeta& meta = ObjectMeta{}, const P& p = {}, string_view poolkey = {})
	{
		auto* obj = svc.Spawn<T, P>(meta, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), string(poolkey));
	}

#pragma endregion
	
#pragma region Fast 썽크 보장
	//Clone(key,P)만 쓰는 특수 타입에서 런타임 썽크 선등록이 필요할 때 사용.
	// 일반적으로는 Spawn<T,P> 또는 CreateProto<T,Pproto>가 자동 등록함.

	template<class T, class P>
	inline void EnsureFastInit(IObjectService& svc) { svc.EnsureFastInit<T, P>(); }

	template<class T, class P>
	inline void EnsureFastProto(IObjectService& svc) { svc.EnsureFastProto<T, P>(); }

#pragma endregion

	/// 스코프 동안 레이어 일시정지
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

	/// 스코프 동안 렌더 정지
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

}