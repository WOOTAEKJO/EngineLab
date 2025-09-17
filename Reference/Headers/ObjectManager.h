#pragma once
#include "ObjectService.h"
#include "IManager.h"

BEGIN(Engine)

// --------- ObjectManager ------------

class CObjectManager : public IManager, public ITickable, public IObjectService
{
protected:
	CObjectManager();
	~CObjectManager();

public: // IManager
	virtual HRESULT Init() override;
	virtual void Tick(Engine::_double dt) override;
	virtual void Shutdown() override;
	
//public: // Factory
//	template<class T, class... A>
//	T* Create(const ObjectMeta& meta, A&&...a)
//	{
//		static_assert(std::is_base_of_v<IGameObject, T>, "T�� IGameObject�� ��Ӱ��谡 �ƴմϴ�.");
//		
//		auto obj = make_unique_enabler_as_builder<IGameObject, T>().Init(forward<A>(a)...);
//		T* raw = static_cast<T*>(obj.get());
//
//		m_store.emplace_back(move(obj)); // �������� ���� ���� �����̳ʿ� �ѱ�
//		m_live.push_back(raw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
//		m_Meta[raw] = meta; // ��ü�� ��Ÿ ������ ����
//		indexAdd(raw, meta); // ���̾� ���� ����
//		
//		raw->OnActivate(); // ��ü Ȱ��ȭ
//		return raw;
//	}
//
//	template<class T,class... A>
//	T* Create(A&&...a)
//	{
//		ObjectMeta meta{};
//		return Create<T>(meta, forward<A>(a)...);
//	}
//
//public: // Prototype
//	template<class T, class... A>
//	_bool RegisterPrototype(const string& key, const ObjectMeta& meta, A&&... a)
//	{
//		static_assert(std::is_base_of_v<IGameObject, T>, "T�� IGameObject�� ��Ӱ��谡 �ƴմϴ�.");
//
//		if (m_Prototype.count(key)) return false; // ��� Ȯ��
//		 auto obj = make_unique_enabler_as_builder<IGameObject,T>().Init(forward<A>(a)...);
//
//		 m_Prototype[key] = move(obj); // ���� ������ �����ϴ� �����̳ʿ� �������� �ѱ�
//		 m_PrototypeMeta[key] = meta; // ��ü�� ��Ÿ ���� ����
//		 return true;
//	}
//
//	template<class T, class... A>
//	_bool RegisterPrototype(const string& key, A&&... a)
//	{
//		ObjectMeta meta{};
//		return RegisterPrototype<T>(key, meta, forward<A>(a)...);
//	}
//
//	template<class... A>
//	IGameObject* Clone(const string& key,A&&... a)
//	{
//		auto it = m_Prototype.find(key);
//		if (it == m_Prototype.end()) return nullptr;
//
//		auto clone = it->second->Clone();
//		if (!clone) return nullptr;
//
//		if (FAILED(clone->InitInstance(forward<A>(a)...)))
//		{
//			/*string str = "Failed to Cloned :";
//			MSG_BOX(str + (key));*/
//			return nullptr;
//		}
//
//		IGameObject* raw = clone.get();
//
//		m_store.emplace_back(move(clone)); // ���� ���� �����̳ʿ� �������� �Ѱ���
//		m_live.push_back(raw);
//
//		auto meta = m_PrototypeMeta[key];
//		m_Meta[raw] = meta; // ��ü�� ��Ÿ ������ ����
//		indexAdd(raw, meta); // �ε��� �� ���̾� ����
//
//		raw->OnActivate(); // ��ü Ȱ��ȭ
//		return raw;
//	}
//
//public: // Pool
//	template<class... A>
//	void Ready_Pool(const string& key, int n,A&&... a)
//	{
//		auto it = m_Prototype.find(key);
//		if (it == m_Prototype.end()) return;
//
//		auto& bucket = m_Pool[key]; // ���� �޸𸮿� ��ȭ�� �־�� �ϱ� ������ ���۷��� ���.
//		auto meta = m_PrototypeMeta[key];
//
//		while (static_cast<int>(bucket.size()) < n) // n �������� ���ڶ� ��ŭ Ŭ�� ����
//		{
//			auto clone = it->second->Clone();
//			if (!clone || FAILED(clone->InitInstance(forward<A>(a)...))) break;
//
//			IGameObject* raw = clone.get();
//
//			m_PoolMeta[raw] = meta; // Ǯ ��Ÿ �����̳ʿ��� ��Ÿ ���� ����
//			bucket.emplace_back(move(clone)); // �������� �ѱ�
//		}
//	}
//
//	IGameObject* Acquire(const string& key); // Ǯ���� ������
	
public:// ObjectService �����ø� ����
	virtual HRESULT DefineSpawnRaw(const TypeBinding& b) override; // Ÿ�� ���
	virtual HRESULT DefinePrototypeInitRaw(const TypeBinding& b) override; // ������Ÿ�� �ʱ�ȭ ���
	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) override; // ������Ÿ�� ���� ���

	virtual IGameObject* SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params) override;
	virtual IGameObject* CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params = {}) override;
	virtual size_t Ready_Pool(string_view key, size_t targetCount) override;
	virtual IGameObject* AcquireFromPool(string_view key, AnyParams params = {}) override;
	virtual void Release(IGameObject* obj, const string& key) override; // �ٽ� Ǯ�� ����������

public:// ��ȸ ��
	virtual const vector<IGameObject*>& ViewByLayer(LayerID id) const override { return m_byLayer[id]; }
	virtual const vector<IGameObject*>& ViewByTag(TagID id) const override
	{
		static const vector<IGameObject*> empty;
		auto it = m_byTag.find(id);
		return (it == m_byTag.end()) ? empty : it->second;
	}

public:// ���� ��Ÿ ����
	virtual void SetLayer(IGameObject* obj, LayerID id) override; // ���̾� id ���� ����
	virtual void SetTag(IGameObject* obj, TagID id) override; // �±� id ���� ����

	virtual void SetVisible(IGameObject* obj, _bool on) override { m_Meta[obj].visible = on; } // ��ü�� visible ����
	virtual void SetPaused(IGameObject* obj, _bool on) override { m_Meta[obj].paused = on; } // ��ü�� paused ����
	virtual void SetTimeScale(IGameObject* obj, _double scale) override { m_Meta[obj].timescale = scale; } // ��ü�� timescale ����

public: // ���̾� ���� ����
	virtual void SetLayerVisible(LayerID id, _bool on) override;
	virtual void SetLayerPaused(LayerID id, _bool on) override;
	virtual void SetLayerTimeScale(LayerID id, _double scale) override;
	// ���������� ����

	virtual _bool LayerVisible(LayerID id) const override { return m_layerVisible[id]; }
	virtual _bool LayerPaused(LayerID id) const override { return m_layerPaused[id]; }
	virtual _double LayerTimeScale(LayerID id) const override { return m_layerScale[id]; }
	// ���� Ȯ��

private:
	static void vecEraseUnordered(vector<IGameObject*>& v, IGameObject* p); // ���� �ȿ� ��ü �����

	void	indexAdd(IGameObject* obj, const ObjectMeta& meta); // ��ü�� ��Ÿ ������ ����
	void	indexRemove(IGameObject* obj, const ObjectMeta& meta); // ��ü�� ��Ÿ ������ ����
	void	indexMoveLayer(IGameObject* obj, LayerID oldID, LayerID newID); // ��ü�� ���̾� ���� ����
	void	indexMoveTag(IGameObject* obj, TagID oldID, TagID newID); // ��ü�� �±� ���� ����
	void	applyLayerFlags(IGameObject* obj, LayerID id); // ���̾� �÷��� ����

private:
	// ����/ Ȱ�� ����
	vector<u_ptr<IGameObject>>	m_store; // ���� ��ü�� ������ �����̳�
	vector<IGameObject*>		m_live; // ���� Ȱ��ȭ�Ǿ� �ִ���.

	// ������Ÿ��
	unordered_map<string, u_ptr<IGameObject>>	m_Prototype; // ������Ÿ�� ���� ����
	unordered_map<string, ObjectMeta>			m_PrototypeMeta; // ������Ÿ�� ��Ÿ

	// Ǯ
	unordered_map<string, vector<u_ptr<IGameObject>>>	m_Pool; // Ǯ�� ��� �ִ� ������Ʈ�� -> ���� �����(��Ȱ��ȭ ����)
	unordered_map<IGameObject*, ObjectMeta>				m_PoolMeta; // Ǯ�� �� ��ü�� ��Ÿ(�ǵ��� �� ����)

	// ��Ÿ/�ε���(���� ��ȸ��)
	unordered_map<IGameObject*, ObjectMeta>		m_Meta; // ��ü�� ��Ÿ -> ��ü�� �ּҸ� Ű������ ��Ÿ ������ �����Ѵ�.
	array<vector<IGameObject*>,32>				m_byLayer; // ���̾ Ȱ�� ��� -> ��ü�� 32���� ���̾��߿��� �����Ͽ� �����Ѵ�.
	unordered_map<TagID, vector<IGameObject*>>	m_byTag; // �ױ׺� Ȱ�� ��� -> ���� �±׺��� ��ü�� �����Ѵ�.

	// ���̾� ���� ����
	array<_bool, 32>	m_layerVisible;
	array<_bool, 32>	m_layerPaused;
	array<_double, 32>	m_layerScale;

private: // "������Ʈ Ÿ��T + �Ķ���� Ÿ��P" �������� ��Ÿ�� ����ġ
	struct Key
	{
		type_index obj;			// ������ "������Ʈ Ÿ��"�� typeid(T)
		//type_index param;		// obj�� ���� type_index�� �����ϸ� �� ������
		const type_info* param; // �߰� �Ķ���� Ÿ���� typeid(p) �ּ� 
		bool operator==(const Key& o) const { return obj == o.obj && param == o.param; }
	};/*
		- obj�� std::type_index�� ���� �� typeid(T) ��/�ؽð� ����.
		- param�� �Ķ���� Ÿ��(��: BulletParams)�� type_info* �����͸� �״�� ����. 
			-> ��, ���� T�� P�� �ٸ��� �ٸ� Key�� �ȴ�.
	  */

	struct KeyHash
	{
		size_t operator()(const Key& k) const noexcept
		{
			return hash<type_index>{}(k.obj) ^ (reinterpret_cast<uintptr_t>(k.param) >> 3);
			// �ؽ�: obj�� �ؽÿ� param ������ ���� ����. >> 3�� ���� 3��Ʈ�� �о� �ణ ���� Ʈ��.

			/*size_t h1 = hash<type_index>{}(k.obj);
			size_t h2 = hash<type_index>{}(k.param);
			return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));*/
			// ����� hash-combine�� ���.
		}	
	};

	unordered_map<Key, CreateFn, KeyHash>			m_createFns; // ���� �Լ� �����͸� ����
	unordered_map<Key, InitFn, KeyHash>				m_initFns; // �ʱ�ȭ �Լ� �����͸� ����
	unordered_map<Key, ProtoInitFn, KeyHash>		m_protoInitFns; // ������Ÿ�� �ʱ�ȭ �Լ� �����͸� ����
};

END
