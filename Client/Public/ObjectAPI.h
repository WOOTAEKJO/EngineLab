#pragma once
#include "Client_Defines.h"
#include "EngineAPI.h"

namespace Engine::API
{
	/// ��Ÿ ���� (ü�̴�)
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

	/// �Ķ���� �ڵ� �߷� ����/ȹ�� ����
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

	/// RAII ��Ÿ���� ��������Ʈ �뿩��(�ڵ�)��
	class objHandle
	{
	public:
		objHandle() = default;
		objHandle(IObjectService& svc,string_view key, IGameObject* p)
			:m_svc(&svc),m_key(string(key)),m_ptr(p){} // �뿩 ����

		objHandle(const objHandle&) = delete;				// ���� ������ ����
		objHandle& operator=(const objHandle&) = delete;	// ���� ������ ����
		// ���� �ݳ��� ���� ����

		objHandle(objHandle&& o) noexcept
			:m_svc(o.m_svc),m_key(o.m_key),m_ptr(exchange(o.m_ptr,nullptr)) {}
		// �̵�����. �������� �ű��, ���� o.m_ptr�� null�� ����� �ݳ� �ߺ� ����

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
		}// �̵� ���� : �ڱ� ���� ���� ��, ���� ���� reset()���� ���� �ݳ��ϰ� �� ���·� ��ü

		~objHandle() { reset(); } // �Ҹ��ڿ��� �ڵ� �ݳ� ȣ��

		void reset()
		{
			if (m_svc && m_ptr) { m_svc->Release(m_ptr, m_key); m_ptr = nullptr; }
			// �ڵ� Reales ȣ��
		}

		IGameObject* get() const { return m_ptr; }					// ���� ������ ����
		IGameObject& operator*() const { return *m_ptr; }			
		IGameObject* operator->() const { return m_ptr; }			// �� �Լ��� ������ó�� ����ϱ� ����
		explicit operator bool() const { return m_ptr != nullptr; }	// ��ȿ ���� üũ

	private:
		IObjectService* m_svc{}; // �ݳ��� ����ϴ� ����
		string			m_key;	 // Ǯ/������Ÿ�� ���� ��Ŷ �ĺ���.
		IGameObject*	m_ptr{}; // ���� ������Ʈ �ּ�. ��ȿ�ϸ� �뿩��
	};

	/// Ǯ ȹ���� �ڵ�� �ٷ� �ޱ�
	template<class P = monostate>
	inline objHandle AcquireHandle(IObjectService& svc, string_view key, const P& p = {})
	{
		if (auto* g = Acquire(key, p)) return objHandle{ svc,key,g };
		return {};
	}

	/// ������ ���� ���̾� �Ͻ�����/ ���ü� ����
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
	
	//Ű-��� ������Ƽ��: ����/������Ÿ�� ������ ������

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