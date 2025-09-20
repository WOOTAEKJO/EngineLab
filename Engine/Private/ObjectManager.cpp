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
	m_createFns.try_emplace(k, fn); // �̹� ������ ���ΰ�, ������ �߰�
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
	if (pit != m_Prototype.end()) return S_OK; // ���: �̹� �����ϰ� ������ ����

	auto it = m_createFns.find(CreateKeyErased(type)); // ��ϵ� ���� �Լ� ������ �˻�
	if (it == m_createFns.end()) return E_FAIL;

	u_ptr<IGameObject> up = it->second(ObjectMeta{}, pack(monostate{})); // �� �Ķ���͸� ���� ��ü ����
	if (!up) return E_FAIL;
	
	HRESULT ok = E_FAIL;

	if (auto init = m_protoInitFns.find(Key{ type,params.type }); init != m_protoInitFns.end())
		ok = init->second(*up, params); // �Ķ���Ͱ� �ִ� ����
	else if (auto init_ = m_protoInitFns.find(Key{ type,type_of<monostate>() }); init_ != m_protoInitFns.end())
		ok = init_->second(*up, AnyParams{ nullptr,0,type_of<monostate>() }); // �Ķ���Ͱ� ���� ����
	else
		ok = up->InitPrototype(); // ���� �� ��찡 �ƴ� ����

	if (FAILED(ok)) return E_FAIL;

	auto cit = m_cloneFns.find(type);
	if (cit == m_cloneFns.end()) return E_FAIL;

	//m_Prototype[string(key)] = move(up); // ���� ��ü ������ �̵�
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
	}// ������ ����. monostate�� ã�ƺ���.

	if (create == m_createFns.end() || init == m_initFns.end()) return nullptr;

	u_ptr<IGameObject> up = create->second(meta, params); // ��ü ����
	if (!up) return nullptr; 
	if (FAILED(init->second(*up, params))) return nullptr; // ��ü InitInstance ȣ��

	IGameObject* raw = up.get();
	size_t idx = m_store.size();

	m_store.emplace_back(ObjRec(move(up),type,meta) ); // �������� ���� ���� �����̳ʿ� �ѱ�
	m_storeIndex[raw] = idx;	// �ε��� ����.
	m_live.push_back(raw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
	indexAdd(raw, meta); // ���̾� ���� ����

	raw->OnActivate(); // ��ü Ȱ��ȭ
	return raw;
}

IGameObject* CObjectManager::CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params)
{
	auto pit = m_Prototype.find(string(key));
	if (pit == m_Prototype.end()) return nullptr;

	ProtoRec& rec = pit->second;

	u_ptr<IGameObject> clone = rec.cloner(*rec.proto); // Ŭ�� ���� �Լ� �����͸� �̿��Ͽ� ����
	if (!clone) return nullptr;

	IGameObject* craw = clone.get();

	//const type_index objType = typeid(*craw); // �������� ��Ÿ�� ���������� Ÿ�� ����
	const type_index objType = rec.type; // ������ ���������� Ÿ�� ������ �˷���� ���� �Լ� ������ �����̳ʿ��� ã�� �� ����.

	HRESULT ok = E_FAIL;

	if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
		ok = init->second(*craw, params); // �Ķ���Ͱ� �ִ� ����
	else if (auto init_ = m_initFns.find(Key{ objType,type_of<monostate>() }); init_ != m_initFns.end())
		ok = init_->second(*craw, AnyParams{ nullptr,0,type_of<monostate>() }); // �Ķ���Ͱ� ���� ����
	else
		ok = craw->InitInstance(); // ���� �� ��찡 �ƴ� ����

	if (FAILED(ok)) return nullptr;

	size_t idx = m_store.size();
	m_store.emplace_back(ObjRec{ move(clone),rec.type,meta }); // �������� ���� ���� �����̳ʿ� �ѱ�
	m_storeIndex[craw] = idx;	// �ε��� ����
	m_live.push_back(craw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
	indexAdd(craw, meta); // ���̾� ���� ����

	craw->OnActivate(); // ��ü Ȱ��ȭ
	return craw;
}

IGameObject* CObjectManager::AcquireFromPool(string_view key, AnyParams params)
{
	auto it = m_Pool.find(string(key));
	if (it != m_Pool.end() && !it->second.empty()) // ��ü�� Ǯ�� �����ϰ� Ǯ ���Ͱ� �ִٸ�
	{
		ObjRec slot = move(it->second.back());
		it->second.pop_back(); // Ǯ���� ����

		u_ptr<IGameObject> up = move(slot.obj); // Ǯ���� ��������
		IGameObject* raw = up.get();

		raw->ResetInstance(); // ���� �Լ� ȣ��

		type_index objType = slot.type;
		HRESULT ok = E_FAIL;

		if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
			ok = init->second(*raw, params); // �Ķ���Ͱ� �ִ� ����
		else if (auto init_ = m_initFns.find(Key{ objType,type_of<monostate>() }); init_ != m_initFns.end())
			ok = init_->second(*raw, AnyParams{ nullptr,0,type_of<monostate>() }); // �Ķ���Ͱ� ���� ����
		else
			ok = raw->InitInstance(); // ���� �� ��찡 �ƴ� ����

		if (FAILED(ok)) return nullptr;

		size_t idx = m_store.size();

		m_store.emplace_back(ObjRec{ move(up),objType,slot.meta }); // �������� ���� ���� �����̳ʿ� �ѱ�
		m_storeIndex[raw] = idx; // �ε��� ����idx
		m_live.push_back(raw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
		indexAdd(raw, slot.meta); // ����� �� ���̾� ����

		raw->OnActivate(); // Ȱ��ȭ
		return raw;
	}

	return CloneFromPrototype(key, ObjectMeta{}, params); // Ǯ�� Ŭ���� ���ٸ� Ŭ�� ���� �Լ� ȣ��. �� ��Ÿ ������ ����ִ�.
}

void CObjectManager::Release(IGameObject* obj, const string& key)
{
	if (!obj) return;
	obj->OnDeActivate(); // ��Ȱ��ȭ

	//Ȱ��ȭ �ε��� ��ȸ
	auto itidx = m_storeIndex.find(obj);
	if (itidx == m_storeIndex.end()) return;

	size_t idx = itidx->second;

	ObjRec& rec = m_store[idx];
	ObjectMeta	meta = rec.meta;

	indexRemove(obj, meta);
	vecEraseUnordered(m_live, obj); // live �����̳ʿ����� ��ü ����.

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

		applyLayerFlags(obj, id); // ���̾� ���� ���׸� ��� �ݿ�
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
	assert(meta.layerID < m_byLayer.size()); // m_byLayer ũ��� 32. ���̾� id�� 32���� ũ�� �ȵȴ�.

	m_byLayer[meta.layerID].push_back(obj);
	m_byTag[meta.tagID].push_back(obj);

	applyLayerFlags(obj, meta.layerID);
}

void CObjectManager::indexRemove(IGameObject* obj, const ObjectMeta& meta)
{
	vecEraseUnordered(m_byLayer[meta.layerID], obj); // ���̾� �ȿ� ��ü �����
	auto it = m_byTag.find(meta.tagID);
	if (it != m_byTag.end())
	{
		vecEraseUnordered(it->second, obj); // Ʈ�� �ȿ� ��ü �����.
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
		- ��ä�� ��Ÿ ������ ���̾� ���� �������� ����
		- �̴� ���� ��Ÿ ������ �ʱ�ȭ �ϴ� ���� �ƴ�, ���̾��� ���� ������ ��ü�� �������� �� ���� ������ ���´ٴ� ���̴�.
			- ���� ���̾ �� �ִ� ��ü �׷��� �� ����ġ�� �¿��Ѵٴ� ��.
		- �̴� ����/���� �������� ���� ����(�ƽ�, �Ͻ�����, ������ ��ȯ)�� "�� �濡" �ɱ� ���� �ǵ��̴�.

	*/
}

void CObjectManager::Shutdown()
{
}
