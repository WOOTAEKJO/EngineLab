#pragma once
#include "ObjectService.h"
#include "IManager.h"

BEGIN(Engine)

struct ObjRec
{
	u_ptr<IGameObject>	obj;	// 인스턴스
	type_index			type;	// 정적 타입 -> 키 값
	ObjectMeta			meta;	// 개체매타
}; // 활성/ 비활성 모두 사용

struct ProtoRec
{
	u_ptr<IGameObject>	proto;	// 원본
	type_index			type;	// 타입정보
	CloneFn				cloner; // 클론생성 함수 포인터
};// 런타임 RTTI로 키를 만들면 종종 실패한다. 때문에 키는 정적 타입으로만 만들고, 조회를 하기 위한 구조체

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
	
public:// ObjectService 비템플릿 가상
	virtual HRESULT DefineSpawnErased(const SpawnBinding& b) override; // 타입 등록
	virtual HRESULT DefinePrototypeInitErased(const ProtoBinding& b) override; // 프로토타입 초기화 등록
	virtual	HRESULT	DefineCloneErased(const CloneBinding& b) override;	// 클론 등록

	virtual void	EnsureCreateErased(type_index obj, CreateFn fn) override;
	
	virtual size_t	PrimeTypeErased(type_index type, string_view key, size_t targetCount, AnyParams params) override;
	virtual HRESULT PrimePrototypeErased(string_view key, size_t targetCount) override;

	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) override; // 프로토타입 생성 등록

	virtual IGameObject* SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params) override;
	virtual IGameObject* CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params = {}) override;
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

	virtual void SetVisible(IGameObject* obj, _bool on) override;// 개체의 visible 변경
	virtual void SetPaused(IGameObject* obj, _bool on) override; // 개체의 paused 변경
	virtual void SetTimeScale(IGameObject* obj, _double scale) override; // 개체의 timescale 변경

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
	inline ObjRec* GetRec(IGameObject* obj)
	{
		auto it = m_storeIndex.find(obj);
		return (it != m_storeIndex.end()) ? &m_store[it->second] : nullptr;
	}// 활성 컨테이너 조회

	static void vecEraseUnordered(vector<IGameObject*>& v, IGameObject* p); // 벡터 안에 개체 지우기

	void	indexAdd(IGameObject* obj, const ObjectMeta& meta); // 개체의 메타 정보를 저장
	void	indexRemove(IGameObject* obj, const ObjectMeta& meta); // 개체의 메타 정보를 삭제
	void	indexMoveLayer(IGameObject* obj, LayerID oldID, LayerID newID); // 개체의 레이어 정보 수정
	void	indexMoveTag(IGameObject* obj, TagID oldID, TagID newID); // 개체의 태그 정보 수정
	void	applyLayerFlags(IGameObject* obj, LayerID id); // 레이어 플레그 적용

private:
	// 소유/ 활성 집합
	vector<ObjRec>								m_store; // 실제 객체를 소유한 컨테이너
	unordered_map<IGameObject*, size_t>			m_storeIndex; // O(1) 조회 Release용 인덱스 테이블
	vector<IGameObject*>						m_live; // 현재 활성화되어 있는지.

	// 프로토타입
	unordered_map<string, ProtoRec, SvHash, SvEq>			m_Prototype; // 프로토타입 원형 집합
	unordered_map<string, ObjectMeta, SvHash, SvEq>			m_PrototypeMeta; // 프로토타입 메타

	// 풀
	unordered_map<string, vector<ObjRec>, SvHash, SvEq>		m_Pool; // 풀에 담겨 있는 오브젝트들 -> 재사용 대기중(비활성화 상태)

	array<vector<IGameObject*>,32>				m_byLayer; // 레이어별 활성 목록 -> 개체를 32개의 레이어중에서 선택하여 저장한다.
	unordered_map<TagID, vector<IGameObject*>>	m_byTag; // 테그별 활성 목록 -> 같은 태그별로 개체를 저장한다.

	// 레이어 전역 상태
	array<_bool, 32>	m_layerVisible;
	array<_bool, 32>	m_layerPaused;
	array<_double, 32>	m_layerScale;

private: // "오브젝트 타입T + 파라미터 타입P" 조합으로 런타임 디스패치
	

	unordered_map<Key, CreateFn, KeyHash>			m_createFns; // 생성 함수 포인터를 보관
	unordered_map<Key, InitFn, KeyHash>				m_initFns; // 초기화 함수 포인터를 보관
	unordered_map<Key, ProtoInitFn, KeyHash>		m_protoInitFns; // 프로토타입 초기화 함수 포인터를 보관
	unordered_map<type_index,CloneFn>				m_cloneFns; // 클론 생성 함수 포인터를 보관
};

END
