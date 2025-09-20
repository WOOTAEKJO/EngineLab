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

HRESULT CObjectManager::DefineSpawnErased(const SpawnBinding& b)
{
	m_initFns[SpawnKeyErased(b.obj, b.param)] = b.init; 

	return S_OK;
}

HRESULT CObjectManager::DefinePrototypeInitErased(const ProtoBinding& b)
{
	m_protoInitFns[Key{ b.obj,b.param }] = b.prototypeInit;
	return S_OK;
}

HRESULT CObjectManager::DefineCloneErased(const CloneBinding& b)
{
	m_cloneFns[b.obj] = b.clone;
	return S_OK;
}

void CObjectManager::EnsureCreateErased(type_index obj, CreateFn fn)
{
	Key k = CreateKeyErased(obj);
	m_createFns.try_emplace(k, fn); // 이미 있으면 나두고, 없으면 추가
}

size_t CObjectManager::PrimeTypeErased(type_index type, string_view key, size_t targetCount, AnyParams params)
{
	auto it_bucket = m_Pool.find(string(key));
	auto& bucket = (it_bucket == m_Pool.end()) ? m_Pool[string(key)] : it_bucket->second;
	if (bucket.size() >= targetCount) return 0;

	auto it = m_createFns.find(CreateKeyErased(type));
	if (it == m_createFns.end()) return 0;

	size_t added = 0;
	ObjectMeta meta = {};

	while (bucket.size() < targetCount)
	{
		u_ptr<IGameObject> up = it->second(meta, params);
		if (!up) break;

		up->ResetInstance();
		bucket.emplace_back(ObjRec{ move(up),type,meta} );
		++added;
	}

	return added;
}

HRESULT CObjectManager::PrimePrototypeErased(string_view key, size_t targetCount)
{
	auto& bucket = m_Pool[string(key)];
	if (bucket.size() >= targetCount) return S_OK;

	auto pit = m_Prototype.find(string(key));
	if (pit == m_Prototype.end()) return E_FAIL;

	ProtoRec& rec = pit->second;
	auto metait = m_PrototypeMeta.find(string(key));
	ObjectMeta meta = (metait != m_PrototypeMeta.end()) ? metait->second : ObjectMeta{};

	while (bucket.size() < targetCount)
	{
		auto clone = rec.cloner(*rec.proto);
		if (!clone) break;

		clone->ResetInstance();
		bucket.emplace_back(ObjRec{ move(clone),rec.type,meta});
	}

	return S_OK;
}

HRESULT CObjectManager::CreatePrototypeByType(type_index type, string_view key, AnyParams params)
{
	auto pit = m_Prototype.find(string(key));
	if (pit != m_Prototype.end()) return S_OK; // 멱등: 이미 존재하고 있으면 리턴

	auto it = m_createFns.find(CreateKeyErased(type)); // 등록된 생성 함수 포인터 검색
	if (it == m_createFns.end()) return E_FAIL;

	u_ptr<IGameObject> up = it->second(ObjectMeta{}, pack(monostate{})); // 빈 파라미터를 가진 객체 생성
	if (!up) return E_FAIL;
	
	HRESULT ok = E_FAIL;

	if (auto init = m_protoInitFns.find(Key{ type,params.type }); init != m_protoInitFns.end())
		ok = init->second(*up, params); // 파라미터가 있는 버전
	else if (auto init_ = m_protoInitFns.find(Key{ type,type_of<monostate>() }); init_ != m_protoInitFns.end())
		ok = init_->second(*up, AnyParams{ nullptr,0,type_of<monostate>() }); // 파라미터가 없는 버전
	else
		ok = up->InitPrototype(); // 위에 두 경우가 아닌 버전

	if (FAILED(ok)) return E_FAIL;

	auto cit = m_cloneFns.find(type);
	if (cit == m_cloneFns.end()) return E_FAIL;

	//m_Prototype[string(key)] = move(up); // 원형 객체 소유권 이동
	m_Prototype.emplace(key, ProtoRec{
			.proto = move(up),.type = type,.cloner = cit->second
		});

	return S_OK;
}

IGameObject* CObjectManager::SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params)
{
	auto create = m_createFns.find(CreateKeyErased(type));
	auto init = m_initFns.find(SpawnKeyErased(type,params.type));

	if (init == m_initFns.end())
	{
		init = m_initFns.find(SpawnKeyErased(type, type_of<monostate>()));
	}// 만약을 위함. monostate로 찾아보기.

	if (create == m_createFns.end() || init == m_initFns.end()) return nullptr;

	u_ptr<IGameObject> up = create->second(meta, params); // 개체 생성
	if (!up) return nullptr; 
	if (FAILED(init->second(*up, params))) return nullptr; // 개체 InitInstance 호출

	IGameObject* raw = up.get();
	size_t idx = m_store.size();

	m_store.emplace_back(ObjRec(move(up),type,meta) ); // 소유권을 실제 저장 컨테이너에 넘김
	m_storeIndex[raw] = idx;	// 인덱스 설정.
	m_live.push_back(raw); // 활성 컨테이너에 주소를 넘김
	indexAdd(raw, meta); // 레이어 정보 세팅

	raw->OnActivate(); // 개체 활성화
	return raw;
}

IGameObject* CObjectManager::CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params)
{
	auto pit = m_Prototype.find(string(key));
	if (pit == m_Prototype.end()) return nullptr;

	ProtoRec& rec = pit->second;

	u_ptr<IGameObject> clone = rec.cloner(*rec.proto); // 클론 생성 함수 포인터를 이용하여 생성
	if (!clone) return nullptr;

	IGameObject* craw = clone.get();

	//const type_index objType = typeid(*craw); // 기존에는 런타임 시점에서의 타입 정보
	const type_index objType = rec.type; // 컴파일 시점에서의 타입 정보를 알려줘야 생성 함수 포인터 컨테이너에서 찾을 수 있음.

	HRESULT ok = E_FAIL;

	if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
		ok = init->second(*craw, params); // 파라미터가 있는 버전
	else if (auto init_ = m_initFns.find(Key{ objType,type_of<monostate>() }); init_ != m_initFns.end())
		ok = init_->second(*craw, AnyParams{ nullptr,0,type_of<monostate>() }); // 파라미터가 없는 버전
	else
		ok = craw->InitInstance(); // 위에 두 경우가 아닌 버전

	if (FAILED(ok)) return nullptr;

	size_t idx = m_store.size();
	m_store.emplace_back(ObjRec{ move(clone),rec.type,meta }); // 소유권을 실제 저장 컨테이너에 넘김
	m_storeIndex[craw] = idx;	// 인덱스 저장
	m_live.push_back(craw); // 활성 컨테이너에 주소를 넘김
	indexAdd(craw, meta); // 레이어 정보 세팅

	craw->OnActivate(); // 개체 활성화
	return craw;
}

IGameObject* CObjectManager::AcquireFromPool(string_view key, AnyParams params)
{
	auto it = m_Pool.find(string(key));
	if (it != m_Pool.end() && !it->second.empty()) // 개체의 풀이 존재하고 풀 벡터가 있다면
	{
		ObjRec slot = move(it->second.back());
		it->second.pop_back(); // 풀에서 삭제

		u_ptr<IGameObject> up = move(slot.obj); // 풀에서 꺼내오기
		IGameObject* raw = up.get();

		raw->ResetInstance(); // 리셋 함수 호출

		type_index objType = slot.type;
		HRESULT ok = E_FAIL;

		if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
			ok = init->second(*raw, params); // 파라미터가 있는 버전
		else if (auto init_ = m_initFns.find(Key{ objType,type_of<monostate>() }); init_ != m_initFns.end())
			ok = init_->second(*raw, AnyParams{ nullptr,0,type_of<monostate>() }); // 파라미터가 없는 버전
		else
			ok = raw->InitInstance(); // 위에 두 경우가 아닌 버전

		if (FAILED(ok)) return nullptr;

		size_t idx = m_store.size();

		m_store.emplace_back(ObjRec{ move(up),objType,slot.meta }); // 소유권을 실제 저장 컨테이너에 넘김
		m_storeIndex[raw] = idx; // 인덱스 저장idx
		m_live.push_back(raw); // 활성 컨테이너에 주소를 넘김
		indexAdd(raw, slot.meta); // 엔딕싱 및 레이어 세팅

		raw->OnActivate(); // 활성화
		return raw;
	}

	return CloneFromPrototype(key, ObjectMeta{}, params); // 풀에 클론이 없다면 클론 생성 함수 호출. 단 메타 정보는 비어있다.
}

void CObjectManager::Release(IGameObject* obj, const string& key)
{
	if (!obj) return;
	obj->OnDeActivate(); // 비활성화

	//활성화 인덱스 조회
	auto itidx = m_storeIndex.find(obj);
	if (itidx == m_storeIndex.end()) return;

	size_t idx = itidx->second;

	ObjRec& rec = m_store[idx];
	ObjectMeta	meta = rec.meta;

	indexRemove(obj, meta);
	vecEraseUnordered(m_live, obj); // live 컨테이너에서도 개체 정리.

	size_t last = m_store.size() - 1;

	if (idx != last)
	{
		m_store[idx] = move(m_store[last]);
		IGameObject* moved = m_store[idx].obj.get();
		m_storeIndex[moved] = idx;
	}

	m_store.pop_back();
	m_storeIndex.erase(itidx);

	m_Pool[key].push_back(ObjRec{ move(rec.obj),rec.type,meta });
}

void CObjectManager::SetLayer(IGameObject* obj, LayerID id)
{
	if (auto* r = GetRec(obj))
	{
		if (r->meta.layerID == id) return;

		indexMoveLayer(obj, r->meta.layerID, id);
		r->meta.layerID = id;

		applyLayerFlags(obj, id); // 레이어 전역 상테를 즉시 반영
 	}	
}

void CObjectManager::SetTag(IGameObject* obj, TagID id)
{
	if (auto* r = GetRec(obj))
	{
		if (r->meta.tagID == id) return;

		indexMoveTag(obj, r->meta.tagID, id);
		r->meta.tagID = id;
	}
}

void CObjectManager::SetVisible(IGameObject* obj, _bool on)
{
	if (auto* r = GetRec(obj)) r->meta.visible = on;
}

void CObjectManager::SetPaused(IGameObject* obj, _bool on)
{
	if (auto* r = GetRec(obj)) r->meta.paused = on;
}

void CObjectManager::SetTimeScale(IGameObject* obj, _double scale)
{
	if (auto* r = GetRec(obj)) r->meta.timescale = scale;
}

void CObjectManager::SetLayerVisible(LayerID id, _bool on)
{
	m_layerVisible[id] = on;
	for (auto* obj : m_byLayer[id])
	{
		if(auto* r = GetRec(obj))
			r->meta.visible = on;
	}
}

void CObjectManager::SetLayerPaused(LayerID id, _bool on)
{
	m_layerPaused[id] = on;
	for (auto* obj : m_byLayer[id])
	{
		if(auto* r = GetRec(obj))
			r->meta.paused = on;
	}
}

void CObjectManager::SetLayerTimeScale(LayerID id, _double scale)
{
	m_layerScale[id] = scale;
	for (auto* obj : m_byLayer[id])
	{
		if(auto* r = GetRec(obj))
			r->meta.timescale = scale;
	}
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
	if (auto* r = GetRec(obj))
	{
		r->meta.visible = m_layerVisible[id];
		r->meta.paused = m_layerPaused[id];
		r->meta.timescale = m_layerScale[id];
	}
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
