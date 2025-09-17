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
//		static_assert(std::is_base_of_v<IGameObject, T>, "T는 IGameObject와 상속관계가 아닙니다.");
//		
//		auto obj = make_unique_enabler_as_builder<IGameObject, T>().Init(forward<A>(a)...);
//		T* raw = static_cast<T*>(obj.get());
//
//		m_store.emplace_back(move(obj)); // 소유권을 실제 저장 컨테이너에 넘김
//		m_live.push_back(raw); // 활성 컨테이너에 주소를 넘김
//		m_Meta[raw] = meta; // 개체의 메타 정보를 저장
//		indexAdd(raw, meta); // 레이어 정보 세팅
//		
//		raw->OnActivate(); // 개체 활성화
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
//		static_assert(std::is_base_of_v<IGameObject, T>, "T는 IGameObject와 상속관계가 아닙니다.");
//
//		if (m_Prototype.count(key)) return false; // 등록 확인
//		 auto obj = make_unique_enabler_as_builder<IGameObject,T>().Init(forward<A>(a)...);
//
//		 m_Prototype[key] = move(obj); // 실제 원형을 저장하는 컨테이너에 소유권을 넘김
//		 m_PrototypeMeta[key] = meta; // 개체의 메타 정보 저장
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
//		m_store.emplace_back(move(clone)); // 실제 저장 컨테이너에 소유권을 넘겨줌
//		m_live.push_back(raw);
//
//		auto meta = m_PrototypeMeta[key];
//		m_Meta[raw] = meta; // 개체의 메타 정보를 저장
//		indexAdd(raw, meta); // 인덱싱 및 레이어 세팅
//
//		raw->OnActivate(); // 개체 활성화
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
//		auto& bucket = m_Pool[key]; // 실제 메모리에 변화가 있어야 하기 때문에 레퍼런스 사용.
//		auto meta = m_PrototypeMeta[key];
//
//		while (static_cast<int>(bucket.size()) < n) // n 갯수에서 모자란 만큼 클론 생성
//		{
//			auto clone = it->second->Clone();
//			if (!clone || FAILED(clone->InitInstance(forward<A>(a)...))) break;
//
//			IGameObject* raw = clone.get();
//
//			m_PoolMeta[raw] = meta; // 풀 메타 컨테이너에도 메타 정보 저장
//			bucket.emplace_back(move(clone)); // 소유권을 넘김
//		}
//	}
//
//	IGameObject* Acquire(const string& key); // 풀에서 가져옴
	
public:// ObjectService 비템플릿 가상
	virtual HRESULT DefineSpawnRaw(const TypeBinding& b) override; // 타입 등록
	virtual HRESULT DefinePrototypeInitRaw(const TypeBinding& b) override; // 프로토타입 초기화 등록
	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) override; // 프로토타입 생성 등록

	virtual IGameObject* SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params) override;
	virtual IGameObject* CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params = {}) override;
	virtual size_t Ready_Pool(string_view key, size_t targetCount) override;
	virtual IGameObject* AcquireFromPool(string_view key, AnyParams params = {}) override;
	virtual void Release(IGameObject* obj, const string& key) override; // 다시 풀로 돌려보내기

public:// 조회 뷰
	virtual const vector<IGameObject*>& ViewByLayer(LayerID id) const override { return m_byLayer[id]; }
	virtual const vector<IGameObject*>& ViewByTag(TagID id) const override
	{
		static const vector<IGameObject*> empty;
		auto it = m_byTag.find(id);
		return (it == m_byTag.end()) ? empty : it->second;
	}

public:// 개별 메타 제어
	virtual void SetLayer(IGameObject* obj, LayerID id) override; // 레이어 id 정보 수정
	virtual void SetTag(IGameObject* obj, TagID id) override; // 태그 id 정보 수정

	virtual void SetVisible(IGameObject* obj, _bool on) override { m_Meta[obj].visible = on; } // 개체의 visible 변경
	virtual void SetPaused(IGameObject* obj, _bool on) override { m_Meta[obj].paused = on; } // 개체의 paused 변경
	virtual void SetTimeScale(IGameObject* obj, _double scale) override { m_Meta[obj].timescale = scale; } // 개체의 timescale 변경

public: // 레이어 전역 제어
	virtual void SetLayerVisible(LayerID id, _bool on) override;
	virtual void SetLayerPaused(LayerID id, _bool on) override;
	virtual void SetLayerTimeScale(LayerID id, _double scale) override;
	// 전역적으로 수정

	virtual _bool LayerVisible(LayerID id) const override { return m_layerVisible[id]; }
	virtual _bool LayerPaused(LayerID id) const override { return m_layerPaused[id]; }
	virtual _double LayerTimeScale(LayerID id) const override { return m_layerScale[id]; }
	// 상태 확인

private:
	static void vecEraseUnordered(vector<IGameObject*>& v, IGameObject* p); // 벡터 안에 개체 지우기

	void	indexAdd(IGameObject* obj, const ObjectMeta& meta); // 개체의 메타 정보를 저장
	void	indexRemove(IGameObject* obj, const ObjectMeta& meta); // 개체의 메타 정보를 삭제
	void	indexMoveLayer(IGameObject* obj, LayerID oldID, LayerID newID); // 개체의 레이어 정보 수정
	void	indexMoveTag(IGameObject* obj, TagID oldID, TagID newID); // 개체의 태그 정보 수정
	void	applyLayerFlags(IGameObject* obj, LayerID id); // 레이어 플레그 적용

private:
	// 소유/ 활성 집합
	vector<u_ptr<IGameObject>>	m_store; // 실제 객체를 소유한 컨테이너
	vector<IGameObject*>		m_live; // 현재 활성화되어 있는지.

	// 프로토타입
	unordered_map<string, u_ptr<IGameObject>>	m_Prototype; // 프로토타입 원형 집합
	unordered_map<string, ObjectMeta>			m_PrototypeMeta; // 프로토타입 메타

	// 풀
	unordered_map<string, vector<u_ptr<IGameObject>>>	m_Pool; // 풀에 담겨 있는 오브젝트들 -> 재사용 대기중(비활성화 상태)
	unordered_map<IGameObject*, ObjectMeta>				m_PoolMeta; // 풀에 들어간 개체의 메타(되돌릴 때 복원)

	// 메타/인덱스(빠른 조회용)
	unordered_map<IGameObject*, ObjectMeta>		m_Meta; // 개체별 메타 -> 개체의 주소를 키값으로 메타 정보를 저장한다.
	array<vector<IGameObject*>,32>				m_byLayer; // 레이어별 활성 목록 -> 개체를 32개의 레이어중에서 선택하여 저장한다.
	unordered_map<TagID, vector<IGameObject*>>	m_byTag; // 테그별 활성 목록 -> 같은 태그별로 개체를 저장한다.

	// 레이어 전역 상태
	array<_bool, 32>	m_layerVisible;
	array<_bool, 32>	m_layerPaused;
	array<_double, 32>	m_layerScale;

private: // "오브젝트 타입T + 파라미터 타입P" 조합으로 런타임 디스패치
	struct Key
	{
		type_index obj;			// 생성할 "오브젝트 타입"의 typeid(T)
		//type_index param;		// obj와 같은 type_index로 변경하면 더 안전함
		const type_info* param; // 추가 파라미터 타입의 typeid(p) 주소 
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
			return hash<type_index>{}(k.obj) ^ (reinterpret_cast<uintptr_t>(k.param) >> 3);
			// 해시: obj의 해시와 param 포인터 값을 조합. >> 3은 하위 3비트를 털어 약간 섞는 트릭.

			/*size_t h1 = hash<type_index>{}(k.obj);
			size_t h2 = hash<type_index>{}(k.param);
			return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));*/
			// 상수로 hash-combine을 사용.
		}	
	};

	unordered_map<Key, CreateFn, KeyHash>			m_createFns; // 생성 함수 포인터를 보관
	unordered_map<Key, InitFn, KeyHash>				m_initFns; // 초기화 함수 포인터를 보관
	unordered_map<Key, ProtoInitFn, KeyHash>		m_protoInitFns; // 프로토타입 초기화 함수 포인터를 보관
};

END
