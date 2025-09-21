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
	/// ��Ÿ ����(ü�̴�)
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

	/// Ÿ�� ��ϸ� (�Ķ���� ����)
	template<class T>
	inline HRESULT Define(IObjectService& svc) { return svc.DefineSpawn<T>(); } 

	/// ������Ÿ�� ���� (T,P) �ڵ� ��ũ ���
	template<class T, class P = monostate>
	inline HRESULT	CreateProto(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.CreatePrototype<T, P>(key, p);
	}

	/// ������Ÿ�� ��� Ǯ ����
	inline HRESULT Prime(IObjectService& svc, string_view key, size_t cnt)
	{
		return svc.PrimePoolProto(key, cnt);
	}

	/// �� ���� -> ��� + ������Ÿ�� ���� + ����(����)
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

	/// Ÿ�� ������ ���⼭ (T,P) �ڵ� ��ũ ���
	template<class T, class P = monostate>
	inline T* Spawn(IObjectService& svc, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Spawn<T,P>(meta, p);
	}

	/// ���̾ �����ϴ� ������
	template<class T, class P = monostate>
	inline T* SpawnOn(IObjectService& svc, LayerID id, const P& p = {})
	{
		Meta m = API::Meta().Layer(id);
		return svc.Spawn<T, P>(m, p);
	}

	/// ������Ÿ�� Ŭ��
	template<class P = monostate>
	inline IGameObject* Clone(IObjectService& svc, string_view key, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		return svc.Clone<P>(key, meta, p);
	}

	/// Ǯ���� ������. ������ Ŭ�� ����
	template<class P = monostate>
	inline IGameObject* Acquire(IObjectService& svc, string_view key, const P& p = {})
	{
		return svc.Acquire<P>(key, p);
	}

	/// ����(Ǯ�� ��ȯ)
	template<class T = IGameObject>
	inline void Release(IObjectService& svc, T* obj, string_view key)
	{
		svc.Release(obj, string(key));
	}

#pragma region RAII �ڵ� ����

	/// Ǯ ��ο��� �ڵ�� �ޱ�(������ ���� �� �ڵ� Release)
	template<class T = IGameObject, class P = monostate>
	inline objHandle<T> AcquireH(IObjectService& svc, string_view key, const P& p = {})
	{
		auto* obj = svc.Acquire<P>(key, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), std::string(key));
	}

	/// Ŭ�� ��ο��� �ڵ�� �ޱ�(������ ���� �� �ڵ� Release)
	template<class T = IGameObject, class P = monostate>
	inline objHandle<T> CloneH(IObjectService& svc, string_view key, const ObjectMeta& meta = ObjectMeta{}, const P& p = {})
	{
		auto* obj = svc.Clone<P>(key, meta, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), string(key));
	}

	/// Ÿ�� ������ �ڵ� (ǮŰ ��å�� �Բ� �� �� �ִ�.)
	template<class T, class P = monostate>
	inline objHandle<T> SpawnH(IObjectService& svc, const ObjectMeta& meta = ObjectMeta{}, const P& p = {}, string_view poolkey = {})
	{
		auto* obj = svc.Spawn<T, P>(meta, p);
		return objHandle<T>(&svc, static_cast<T*>(obj), string(poolkey));
	}

#pragma endregion
	
#pragma region Fast ��ũ ����
	//Clone(key,P)�� ���� Ư�� Ÿ�Կ��� ��Ÿ�� ��ũ ������� �ʿ��� �� ���.
	// �Ϲ������δ� Spawn<T,P> �Ǵ� CreateProto<T,Pproto>�� �ڵ� �����.

	template<class T, class P>
	inline void EnsureFastInit(IObjectService& svc) { svc.EnsureFastInit<T, P>(); }

	template<class T, class P>
	inline void EnsureFastProto(IObjectService& svc) { svc.EnsureFastProto<T, P>(); }

#pragma endregion

	/// ������ ���� ���̾� �Ͻ�����
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

	/// ������ ���� ���� ����
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