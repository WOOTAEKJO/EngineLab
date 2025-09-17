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
//		bucket.pop_back(); // Ǯ���� �����ͼ� �������� �Ѱ��ְ� ����
//		IGameObject* raw = clone.get();
//
//		// ��Ÿ ���� + �ε���
//		ObjectMeta meta = m_PoolMeta[raw];
//		m_store.emplace_back(move(clone)); // ������ Ŭ���� ���� ���� �����̳ʿ� ����
//		m_live.push_back(raw); // �ּҵ�
//		m_Meta[raw] = meta; // ��Ÿ ������
//		indexAdd(raw, meta); // ����� �� ���̾� ����
//
//		raw->OnActivate(); // Ȱ��ȭ
//		return raw;
//	}
//
//	return Clone(key); // Ǯ�� ����ٸ� ������Ÿ�Կ��� Ŭ���Ͽ� ����.
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
	Key k{ type,&typeid(monostate) }; // �� �Ķ���͸� ���� ��ü Ÿ�� Ű ����

	auto it = m_createFns.find(k); // ��ϵ� ���� �Լ� ������ �˻�
	if (it == m_createFns.end()) return E_FAIL;

	u_ptr<IGameObject> up = it->second(ObjectMeta{}, AnyParams{ nullptr,0,&typeid(monostate) }); // �� �Ķ���͸� ���� ��ü ����
	if (!up) return E_FAIL;
	
	HRESULT ok = E_FAIL;

	if (auto init = m_protoInitFns.find(Key{ type,params.type }); init != m_protoInitFns.end())
		ok = init->second(*up, params); // �Ķ���Ͱ� �ִ� ����
	else if (auto init_ = m_protoInitFns.find(Key{ type,&typeid(monostate) }); init_ != m_protoInitFns.end())
		ok = init_->second(*up, AnyParams{ nullptr,0,&typeid(monostate) }); // �Ķ���Ͱ� ���� ����
	else
		ok = up->InitPrototype(); // ���� �� ��찡 �ƴ� ����

	if (FAILED(ok)) return E_FAIL;

	m_Prototype[string(key)] = move(up); // ���� ��ü ������ �̵�

	return S_OK;
}

IGameObject* CObjectManager::SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params)
{
	Key k{ type,params.type };

	auto create = m_createFns.find(k);
	auto init = m_initFns.find(k);
	if (create == m_createFns.end() || init == m_initFns.end()) return nullptr;

	u_ptr<IGameObject> up = create->second(meta, params); // ��ü ����
	if (!up) return nullptr; 
	if (FAILED(init->second(*up, params))) return nullptr; // ��ü InitInstance ȣ��

	IGameObject* raw = up.get();

	m_store.emplace_back(move(up)); // �������� ���� ���� �����̳ʿ� �ѱ�
	m_live.push_back(raw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
	m_Meta[raw] = meta; // ��ü�� ��Ÿ ������ ����
	indexAdd(raw, meta); // ���̾� ���� ����

	raw->OnActivate(); // ��ü Ȱ��ȭ
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

	u_ptr<IGameObject> clone = cit->second(*praw); // Ŭ�� ���� �Լ� �����͸� �̿��Ͽ� ����
	if (!clone) return nullptr;

	IGameObject* craw = clone.get();

	const type_index objType = typeid(*craw);
	HRESULT ok = E_FAIL;

	if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
		ok = init->second(*craw, params); // �Ķ���Ͱ� �ִ� ����
	else if (auto init_ = m_initFns.find(Key{ objType,&typeid(monostate) }); init_ != m_initFns.end())
		ok = init_->second(*craw, AnyParams{ nullptr,0,&typeid(monostate) }); // �Ķ���Ͱ� ���� ����
	else
		ok = craw->InitInstance(); // ���� �� ��찡 �ƴ� ����

	if (FAILED(ok)) return nullptr;

	m_store.emplace_back(move(clone)); // �������� ���� ���� �����̳ʿ� �ѱ�
	m_live.push_back(craw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�
	m_Meta[craw] = meta; // ��ü�� ��Ÿ ������ ����
	indexAdd(craw, meta); // ���̾� ���� ����

	craw->OnActivate(); // ��ü Ȱ��ȭ
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

		m_PoolMeta[raw] = meta; // Ǯ ��Ÿ �����̳ʿ��� ��Ÿ ���� ����
		bucket.emplace_back(move(clone));
		++added;
	}

	return added;
}

IGameObject* CObjectManager::AcquireFromPool(string_view key, AnyParams params)
{
	auto it = m_Pool.find(string(key));
	if (it != m_Pool.end() && !it->second.empty()) // ��ü�� Ǯ�� �����ϰ� Ǯ ���Ͱ� �ִٸ�
	{
		u_ptr<IGameObject> up = move(it->second.back()); // Ǯ���� ��������
		it->second.pop_back(); // Ǯ���� ����
		IGameObject* raw = up.get();

		raw->ResetInstance(); // ���� �Լ� ȣ��

		type_index objType = typeid(*raw);
		HRESULT ok = E_FAIL;

		if (auto init = m_initFns.find(Key{ objType,params.type }); init != m_initFns.end())
			ok = init->second(*raw, params); // �Ķ���Ͱ� �ִ� ����
		else if (auto init_ = m_initFns.find(Key{ objType,&typeid(monostate) }); init_ != m_initFns.end())
			ok = init_->second(*raw, AnyParams{ nullptr,0,&typeid(monostate) }); // �Ķ���Ͱ� ���� ����
		else
			ok = raw->InitInstance(); // ���� �� ��찡 �ƴ� ����

		if (FAILED(ok)) return nullptr;

		m_store.emplace_back(move(up)); // �������� ���� ���� �����̳ʿ� �ѱ�
		m_live.push_back(raw); // Ȱ�� �����̳ʿ� �ּҸ� �ѱ�

		ObjectMeta m{};
		if (auto mit = m_PoolMeta.find(raw); mit != m_PoolMeta.end())
		{
			m = mit->second;
			m_PoolMeta.erase(mit);
		}

		m_Meta[raw] = m;
		indexAdd(raw, m); // ����� �� ���̾� ����

		raw->OnActivate(); // Ȱ��ȭ
		return raw;
	}

	return CloneFromPrototype(key, ObjectMeta{}, params); // Ǯ�� Ŭ���� ���ٸ� Ŭ�� ���� �Լ� ȣ��. �� ��Ÿ ������ ����ִ�.
}

void CObjectManager::Release(IGameObject* obj, const string& key)
{
	if (!obj) return;
	obj->OnDeActivate(); // ��Ȱ��ȭ

	auto itM = m_Meta.find(obj);
	if (itM != m_Meta.end())
	{
		indexRemove(obj, itM->second);
		m_PoolMeta[obj] = itM->second; // �ٽ� Ǯ�� ���� �־�� �ϴ�. Ǯ��Ÿ ������ ����.
		m_Meta.erase(itM);
	}

	auto it = find_if(m_store.begin(), m_store.end(),
		[&](const u_ptr<IGameObject>& p) {return p.get() == obj; });
	if (it != m_store.end())
	{
		auto up = move(*it); // store���� �������� �ѱ�
		m_store.erase(it); // �ѱ� �������� ��ü�� ����
		m_Pool[key].push_back(move(up)); // ���������� Ǯ�� ���ͷ� �������� �ѱ�
	}

	vecEraseUnordered(m_live, obj); // live �����̳ʿ����� ��ü ����.
}

void CObjectManager::SetLayer(IGameObject* obj, LayerID id)
{
	auto& meta = m_Meta[obj];
	if (meta.layerID == id) return;

	indexMoveLayer(obj, meta.layerID, id);
	meta.layerID = id;

	applyLayerFlags(obj, id); // ���̾� ���� ���׸� ��� �ݿ�
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
	m_Meta[obj].visible = m_layerVisible[id];
	m_Meta[obj].paused = m_layerPaused[id];
	m_Meta[obj].timescale = m_layerScale[id];
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
