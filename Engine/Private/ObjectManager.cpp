#include "../Public/ObjectManager.h"

IObjectService::IObjectService() = default;
IObjectService::~IObjectService() = default;

CObjectManager::CObjectManager()
{
	m_layerVisible.fill(true);
	m_layerPaused.fill(false);
	m_layerScale.fill(1.f);
}

CObjectManager::~CObjectManager()
{
}

HRESULT CObjectManager::Init()
{
	return S_OK;
}

void CObjectManager::Tick(Engine::_double dt)
{
}

//IGameObject* CObjectManager::Acquire(const string& key)
//{
//	auto& bucket = m_Pool[key];
//	if (!bucket.empty())
//	{
//		auto clone = move(bucket.back());
//		bucket.pop_back(); // 풀에서 꺼내와서 소유권을 넘겨주고 삭제
//		IGameObject* raw = clone.get();
//
//		// 메타 복원 + 인덱싱
//		ObjectMeta meta = m_PoolMeta[raw];
//		m_store.emplace_back(move(clone)); // 꺼내온 클론을 실제 관리 컨테이너에 저장
//		m_live.push_back(raw); // 주소도
//		m_Meta[raw] = meta; // 메타 정보도
//		indexAdd(raw, meta); // 엔딕싱 및 레이어 세팅
//
//		raw->OnActivate(); // 활성화
//		return raw;
//	}
//
//	return Clone(key); // 풀이 비었다면 프로토타입에서 클론하여 리턴.
//}

HRESULT CObjectManager::DefineSpawnRaw(const SpawnBinding& b)
{
	Key k{ b.obj,b.param };
	m_createFns[k] = b.create;
	m_initFns[k] = b.init;

	return S_OK;
}

HRESULT CObjectManager::DefinePrototypeInitRaw(const ProtoBinding& b)
{
	m_protoInitFns[Key{ b.obj,b.param }] = b.prototypeInit;
	return S_OK;
}

HRESULT CObjectManager::DefineCloneRaw(const CloneBinding& b)
{
	m_cloneFns[b.obj] = b.clone;
	return S_OK;
}

HRESULT CObjectManager::CreatePrototypeByType(type_index type, string_view key, AnyParams params)
{
	Key k{ type,&typeid(monostate) }; // 빈 파라미터를 가진 객체 타입 키 생성

	auto it = m_createFns.find(k); // 등록된 생성 함수 포인터 검색
	if (it == m_createFns.end()) return E_FAIL;

	u_ptr<IGameObject> up = it->second(ObjectMeta{}, AnyParams{ nullptr,0,&typeid(monostate) }); // 빈 파라미터를 가진 객체 생성
	if (!up) return E_FAIL;
	
	HRESULT ok = E_FAIL;

	if (auto init = m_protoInitFns.find(Key{ type,params.type }); init != m_protoInitFns.end())
		ok = init->second(*up, params); // 파라미터가 있는 버전
	else if (auto init_ = m_protoInitFns.find(Key{ type,&typeid(monostate) }); init_ != m_protoInitFns.end())
		ok = init_->second(*up, AnyParams{ nullptr,0,&typeid(monostate) }); // 파라미터가 없는 버전
	else
		ok = up->InitPrototype(); // 위에 두 경우가 아닌 버전

	if (FAILED(ok)) return E_FAIL;

	m_Prototype[string(key)] = move(up); // 원형 객체 소유권 이동

	return S_OK;
}

IGameObject* CObjectManager::SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params)
{
	Key k{ type,params.type };

	auto create = m_createFns.find(k);
	auto init = m_initFns.find(k);
	if (create == m_createFns.end() || init == m_initFns.end()) return nullptr;

	u_ptr<IGameObject> up = create->second(meta, params); // 개체 생성
	if (!up) return nullptr; 
	if (FAILED(init->second(*up, params))) return nullptr; // 개체 InitInstance 호출

	IGameObject* raw = up.get();

	m_store.emplace_back(move(up)); // 소유권을 실제 저장 컨테이너에 넘김
	m_live.push_back(raw); // 활성 컨테이너에 주소를 넘김
	m_Meta[raw] = meta; // 개체의 메타 정보를 저장
	indexAdd(raw, meta); // 레이어 정보 세팅

	raw->OnActivate(); // 개체 활성화
	return raw;
}

IGameObject* CObjectManager::CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params)
{
	auto pit = m_Prototype.find(string(key));
	if (pit == m_Prototype.end()) return nullptr;

	/*auto clone = it->second->Clone();
	if (!clone) return nullptr;
	IGameObject* raw = clone.get();*/

	IGameObject* praw = pit->second.get();
	auto cit = m_cloneFns.find(typeid(*praw));
	if (cit == m_cloneFns.end()) return nullptr;

	u_ptr<IGameObject> clone = cit->second(*praw); // 클론 생성 함수 포인터를 이용하여 생성
	if (!clone) return nullptr;

	IGameObject* craw = clone.get();

	const type_index objType = typeid(*craw);
	HRESULT ok = E_FAIL;

	if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
		ok = init->second(*craw, params); // 파라미터가 있는 버전
	else if (auto init_ = m_initFns.find(Key{ objType,&typeid(monostate) }); init_ != m_initFns.end())
		ok = init_->second(*craw, AnyParams{ nullptr,0,&typeid(monostate) }); // 파라미터가 없는 버전
	else
		ok = craw->InitInstance(); // 위에 두 경우가 아닌 버전

	if (FAILED(ok)) return nullptr;

	m_store.emplace_back(move(clone)); // 소유권을 실제 저장 컨테이너에 넘김
	m_live.push_back(craw); // 활성 컨테이너에 주소를 넘김
	m_Meta[craw] = meta; // 개체의 메타 정보를 저장
	indexAdd(craw, meta); // 레이어 정보 세팅

	craw->OnActivate(); // 개체 활성화
	return craw;
}

size_t CObjectManager::Ready_Pool(string_view key, size_t targetCount)
{
	auto& bucket = m_Pool[string(key)];
	if (bucket.size() >= targetCount) return 0;

	auto pit = m_Prototype.find(string(key));
	if (pit == m_Prototype.end()) return 0;

	IGameObject* praw = pit->second.get();
	auto cit = m_cloneFns.find(typeid(*praw));
	if (cit == m_cloneFns.end()) return 0;

	auto meta = m_PrototypeMeta[string(key)];

	size_t added = 0;
	const size_t need = targetCount - bucket.size();
	for (size_t i = 0; i < need; i++)
	{
		auto clone = cit->second(*praw);
		if (!clone) break;

		clone->ResetInstance();

		IGameObject* raw = clone.get();

		m_PoolMeta[raw] = meta; // 풀 메타 컨테이너에도 메타 정보 저장
		bucket.emplace_back(move(clone));
		++added;
	}

	return added;
}

IGameObject* CObjectManager::AcquireFromPool(string_view key, AnyParams params)
{
	auto it = m_Pool.find(string(key));
	if (it != m_Pool.end() && !it->second.empty()) // 개체의 풀이 존재하고 풀 벡터가 있다면
	{
		u_ptr<IGameObject> up = move(it->second.back()); // 풀에서 꺼내오기
		it->second.pop_back(); // 풀에서 삭제
		IGameObject* raw = up.get();

		raw->ResetInstance(); // 리셋 함수 호출

		type_index objType = typeid(*raw);
		HRESULT ok = E_FAIL;

		if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
			ok = init->second(*raw, params); // 파라미터가 있는 버전
		else if (auto init_ = m_initFns.find(Key{ objType,&typeid(monostate) }); init_ != m_initFns.end())
			ok = init_->second(*raw, AnyParams{ nullptr,0,&typeid(monostate) }); // 파라미터가 없는 버전
		else
			ok = raw->InitInstance(); // 위에 두 경우가 아닌 버전

		if (FAILED(ok)) return nullptr;

		m_store.emplace_back(move(up)); // 소유권을 실제 저장 컨테이너에 넘김
		m_live.push_back(raw); // 활성 컨테이너에 주소를 넘김

		ObjectMeta m{};
		if (auto mit = m_PoolMeta.find(raw); mit != m_PoolMeta.end())
		{
			m = mit->second;
			m_PoolMeta.erase(mit);
		}

		m_Meta[raw] = m;
		indexAdd(raw, m); // 엔딕싱 및 레이어 세팅

		raw->OnActivate(); // 활성화
		return raw;
	}

	return CloneFromPrototype(key, ObjectMeta{}, params); // 풀에 클론이 없다면 클론 생성 함수 호출. 단 메타 정보는 비어있다.
}

void CObjectManager::Release(IGameObject* obj, const string& key)
{
	if (!obj) return;
	obj->OnDeActivate(); // 비활성화

	auto itM = m_Meta.find(obj);
	if (itM != m_Meta.end())
	{
		indexRemove(obj, itM->second);
		m_PoolMeta[obj] = itM->second; // 다시 풀에 집어 넣어야 하니. 풀메타 정보에 저장.
		m_Meta.erase(itM);
	}

	auto it = find_if(m_store.begin(), m_store.end(),
		[&](const u_ptr<IGameObject>& p) {return p.get() == obj; });
	if (it != m_store.end())
	{
		auto up = move(*it); // store에서 소유권을 넘김
		m_store.erase(it); // 넘긴 소유권의 개체를 삭제
		m_Pool[key].push_back(move(up)); // 최종적으로 풀의 벡터로 소유권을 넘김
	}

	vecEraseUnordered(m_live, obj); // live 컨테이너에서도 개체 정리.
}

void CObjectManager::SetLayer(IGameObject* obj, LayerID id)
{
	auto& meta = m_Meta[obj];
	if (meta.layerID == id) return;

	indexMoveLayer(obj, meta.layerID, id);
	meta.layerID = id;

	applyLayerFlags(obj, id); // 레이어 전역 상테를 즉시 반영
}

void CObjectManager::SetTag(IGameObject* obj, TagID id)
{
	auto& meta = m_Meta[obj];
	if (meta.tagID == id) return;

	indexMoveTag(obj, meta.tagID, id);
	meta.tagID = id;
}

void CObjectManager::SetLayerVisible(LayerID id, _bool on)
{
	m_layerVisible[id] = on;
	for (auto* obj : m_byLayer[id])
		m_Meta[obj].visible = on;
}

void CObjectManager::SetLayerPaused(LayerID id, _bool on)
{
	m_layerPaused[id] = on;
	for (auto* obj : m_byLayer[id])
		m_Meta[obj].paused = on;
}

void CObjectManager::SetLayerTimeScale(LayerID id, _double scale)
{
	m_layerScale[id] = scale;
	for (auto* obj : m_byLayer[id])
		m_Meta[obj].timescale = scale;
}

void CObjectManager::vecEraseUnordered(vector<IGameObject*>& v, IGameObject* p)
{
	auto it = find(v.begin(), v.end(), p);
	if (it != v.end())
	{
		*it = v.back();
		v.pop_back();
	}
}

void CObjectManager::indexAdd(IGameObject* obj, const ObjectMeta& meta)
{
	assert(meta.layerID < m_byLayer.size()); // m_byLayer 크기는 32. 레이어 id가 32보다 크면 안된다.

	m_byLayer[meta.layerID].push_back(obj);
	m_byTag[meta.tagID].push_back(obj);

	applyLayerFlags(obj, meta.layerID);
}

void CObjectManager::indexRemove(IGameObject* obj, const ObjectMeta& meta)
{
	vecEraseUnordered(m_byLayer[meta.layerID], obj); // 레이어 안에 개체 지우기
	auto it = m_byTag.find(meta.tagID);
	if (it != m_byTag.end())
	{
		vecEraseUnordered(it->second, obj); // 트그 안에 개체 지우기.
	}
}

void CObjectManager::indexMoveLayer(IGameObject* obj, LayerID oldID, LayerID newID)
{
	vecEraseUnordered(m_byLayer[oldID], obj);
	m_byLayer[newID].push_back(obj);
}

void CObjectManager::indexMoveTag(IGameObject* obj, TagID oldID, TagID newID)
{
	auto it = m_byTag.find(oldID);
	if (it != m_byTag.end()) vecEraseUnordered(it->second, obj);
	m_byTag[newID].push_back(obj);
}

void CObjectManager::applyLayerFlags(IGameObject* obj, LayerID id)
{
	m_Meta[obj].visible = m_layerVisible[id];
	m_Meta[obj].paused = m_layerPaused[id];
	m_Meta[obj].timescale = m_layerScale[id];
	/*
		- 개채의 메타 정보를 레이어 전역 설정으로 저장
		- 이는 개별 메타 정보를 초기화 하는 것이 아닌, 레이어의 전역 설정이 개체의 설정보다 더 강한 권한을 갖는다는 뜻이다.
			- 같은 레이어에 모여 있는 개체 그룹을 한 스위치로 좌우한다는 것.
		- 이는 연출/디렉팅 관점에서 전역 제어(컷신, 일시정지, 페이즈 전환)를 "한 방에" 걸기 위한 의도이다.

	*/
}

void CObjectManager::Shutdown()
{
}
