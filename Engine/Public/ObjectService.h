#pragma once
#include "IGameObject.h"
#include "KeyUtils.h"

BEGIN(Engine)

/*
	   - ���� ������Ʈ���� ������Ʈ �Ŵ����� ����ϱ� ���� �غ�
		   -> �ٸ� �Ŵ����ʹ� �ٸ��� ������Ʈ �Ŵ����� ���ø� �Լ��� �ټ� ����ϱ� ����
			   -> �̴� �����Լ��� ���ø����� ������ ���ϴ� ������ ���� ũ��.
				   -> vtable���� "������ �ñ״�ó"�� �ö� �� �ִ�.

	   - ����ϱ� ���� ���� -> "�ܺ� API�� ���ø�ó�� ���ϰ�" + "���δ� �������� ����"�� ���ÿ� ����
		   - vtable���� �����ø� ���� �Լ�(��Ÿ�� ����ġ)�� �ø���
		   - �ٱ��ʿ��� ���� ���ø� ���۸� �ּ� ȣ�� ���Ǹ� �� ����,
		   - ���� ���ڴ� Ÿ�� �Ұ�(AnyParams)�� �Ѱ� ��Ÿ�ӿ� ������ �����Ѵ�.
*/

using CreateFn = u_ptr<IGameObject>(*)(const ObjectMeta&,AnyParams);		// �� ��ü ������(InitInstance �� onActivate x)
using CloneFn = u_ptr<IGameObject>(*)(const IGameObject&);					// Ŭ�� ���� ���� �Լ� ������
using InitFn = HRESULT(*)(IGameObject&, AnyParams);	// (������Ʈ/ �Ķ����) �ʱ�ȭ �Լ� ������. Ȯ�� �� InitInstance ȣ��
using ProtoInitFn = HRESULT(*)(IGameObject&, AnyParams);	// ������Ÿ�� InitPrototype ȣ��
															// ȣ���Լ��� �ǹ̰� ���� �� �����Ƿ� ���� �д�.
// - ���� �Լ� ������ ���
// - ������ �ʱ�ȭ�� ������ ������ �⺻/������Ÿ��/Ǯ���� �Ҹ� ��, �� �� Ȥ�� �ϳ��� ȣ���ϱ� ����.

struct SpawnBinding // Ŭ���̾�Ʈ�� ����� ���δ� ����
{
	type_index			obj{typeid(void)};	// ������Ʈ Ÿ��
	type_index			param{typeid(void)};	// ������Ʈ �Ķ����

	CreateFn			create{nullptr};
	InitFn				init{nullptr};

	/*
		��) ���� �Լ��� ����
			ObjUPtr MakeBullet(); // ������ ObjUPtr ��ȯ
			bool InitBullet(IGameObject& o, AnyParams ap); // ���ο��� Bullet�� ĳ�����ؼ� �ʱ�ȭ

			TypeBinding b{
			    std::type_index{typeid(Bullet)},
			    std::type_index{typeid(BulletSpawn)},
			    &MakeBullet,   // construct
			    &InitBullet    // init
			};
			
			// ȣ��
			ObjUPtr up = b.construct();          // ���� �����
			bool ok    = b.init(*up, params);    // �Ķ���ͷ� �ʱ�ȭ
	*/
};

struct ProtoBinding
{
	type_index			obj{ typeid(void) };	// ������Ʈ Ÿ��
	type_index			param{typeid(void)};	// ������Ʈ �Ķ����

	ProtoInitFn			prototypeInit{nullptr};
};

struct CloneBinding
{
	type_index			obj{typeid(void)};	// ������Ʈ Ÿ��

	CloneFn				clone{nullptr};
};

// --------- ObjectService ------------
struct _declspec(novtable) ENGINE_DLL IObjectService
{
protected:
	IObjectService();
	virtual ~IObjectService();

protected: // �����ø� ���� �Լ�: "Ÿ�� �Ұ� + ��Ÿ�� ����ġ"�� �ٽ�
	virtual IGameObject* SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params) = 0;
	virtual IGameObject* CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params = {}) = 0;
	
	virtual IGameObject* AcquireFromPool(string_view key, AnyParams params = {}) = 0;
	
	virtual HRESULT DefineSpawnErased(const SpawnBinding& b) = 0;
	virtual HRESULT DefinePrototypeInitErased(const ProtoBinding& b) = 0;
	virtual	HRESULT	DefineCloneErased(const CloneBinding& b) = 0;

	virtual void	EnsureCreateErased(type_index obj, CreateFn fn) = 0;

	virtual size_t	PrimeTypeErased(type_index type, string_view key, size_t targetCount, AnyParams params) = 0;
	virtual HRESULT PrimePrototypeErased(string_view key,size_t targetCount) = 0;

	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) = 0;

	/*
		- ���� + �����ø����� vtable�� �ö󰣴�.
		- ���� Ÿ���� type_index�� AnyParams�� ��Ÿ�� ������ �ѱ��.
	*/

public:
	//virtual size_t Ready_Pool(string_view key, size_t targetCount) = 0;
	//virtual size_t Ready_Pool_Proto(string_view key, size_t targetCount) = 0;
	virtual void Release(IGameObject* obj, const string& key) = 0;

public: // NOTE: ���ø� ����: ȣ�� ���Ǹ� ����(������ Ÿ�� Ÿ���߷�)
	
	/// ������Ʈ ��Ÿ�� Ȱ��ȭ
	template<class T, class P = std::monostate>
	T* Spawn(const ObjectMeta& meta, const P& p = {})
	{
		return static_cast<T*>(SpawnByType(typeid(T), meta, pack(p)));
	}
	/* NOTE:
		- P
			-> ���⼭ P�� T�� ������ ��, ���� �ѱ�� �Ķ���� ���̴�.
			-> P�� �⺻������ monostateŸ��. monostate-> �� Ÿ���̴�.
		- pack(p)
			-> p�� AnyParams���� Ÿ�� �Ұŷ� �ٲ� ��Ÿ�� ����ġ�� �ѱ��.
	*/

	/// ������Ÿ�� Ŭ�� ��Ÿ�� Ȱ��ȭ
	template<class P = std::monostate>
	IGameObject* Clone(string_view key, const ObjectMeta& meta, const P& p = {})
	{
		return CloneFromPrototype(key, meta, pack(p));
	}

	/// Ǯ �� ������Ʈ ��Ÿ�� Ȱ��ȭ
	template<class P = std::monostate>
	IGameObject* Acquire(string_view key, const P& p = {})
	{
		return AcquireFromPool(key, pack(p));
	}

	/// Ÿ�� ��� ���� -> ������Ÿ�� ��� ����.
	template<class T, class P = monostate>
	size_t PrimePool(string_view key, size_t targetCount, const P& p = {})
	{
		//DefineSpawn<T, P>();
		CreateFn fn = +[](const ObjectMeta& meta, AnyParams ap) ->u_ptr<IGameObject>
		{
			return make_unique_enabler_as_builder<IGameObject, T>().build();
		};

		EnsureCreateErased(typeid(T), fn);
		return PrimeTypeErased(typeid(T), key, targetCount, pack(p));
	}

	/// ������Ÿ�� ��� ����
	HRESULT PrimePoolProto(string_view key, size_t targetCount)
	{
		return PrimePrototypeErased(key, targetCount);
	}

	/// ��Ģ �� �ݹ� ���
	template<class T, class P = monostate> // ���ø� ����
	HRESULT DefineSpawn()
	{
		SpawnBinding bind;
		bind.obj = typeid(T);
		bind.param = type_of<P>();

		bind.create = [](const ObjectMeta& meta, AnyParams ap) -> u_ptr<IGameObject>
		{
			//(void)meta;
			//if (ap.type != type_of<P>()) return {};
			return make_unique_enabler_as_builder<IGameObject, T>().build();
		};

		bind.init = [](IGameObject& obj, AnyParams ap) ->HRESULT
		{
			if (ap.type != type_of<P>()) return E_FAIL;

			if constexpr (is_same_v<P, monostate>) // �Ķ���Ͱ� ���� ��
			{
				return static_cast<IGameObject&>(obj).InitInstance(); // ���� ȣ��θ� ó��(�����ε� ���� ���� ȸ��)
			}
			else // �Ķ���Ͱ� ���� ��
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitInstance(p ? *p : P{});
				return E_FAIL;
			}
		};

		EnsureCreateErased(typeid(T), bind.create); // CreateKey�� �����ϸ鼭 ���� �Լ� ������ �����̳ʿ� ���

		//HRESULT ok = DefineSpawnErased(bind);
		if (FAILED(DefineSpawnErased(bind))) return E_FAIL; // �ʱ�ȭ �Լ� ������ �����̳ʸ� ���. ���⼭ ������ Ű�� SpawnKey

		CloneBinding cb;
		cb.obj = typeid(T);
		cb.clone = [](const IGameObject& obj) ->u_ptr<IGameObject>
		{
			return clone_unique_enabler<IGameObject, T>(static_cast<const T&>(obj));
		};

		// Ŭ�� Ŭ���� �ڵ� ���.
		return DefineCloneErased(cb);  // Ŭ�� ���� �Լ� ������ �����̳ʿ� ���/ Ű ���� obj Ÿ�� �ϳ���.
	}

	/// ������Ÿ�� �ʱ�ȭ ��Ģ �� �ݹ� ���
	template<class T, class P = monostate>
	HRESULT DefinePrototypeInit()
	{
		ProtoBinding bind;
		bind.obj = typeid(T);
		bind.param = type_of<P>();

		bind.prototypeInit = [](IGameObject& obj, AnyParams ap)
		{
			if (ap.type != type_of<P>()) return E_FAIL;

			if constexpr (is_same_v<P, monostate>) // �Ķ���Ͱ� ���� ��
			{
				return static_cast<IGameObject&>(obj).InitPrototype(); // ���� ȣ��θ� ó��(�����ε� ���� ���� ȸ��)
			}
			else // �Ķ���Ͱ� ���� ��
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitPrototype(p ? *p : P{});
				return E_FAIL;
			}
		};

		return DefinePrototypeInitErased(bind);
	}

	/// ������Ÿ�� ��ü�� ����
	template<class T, class P = monostate>
	HRESULT CreatePrototype(string_view key, const P& p = {})
	{
		return CreatePrototypeByType(typeid(T), key,pack(p));
	}

public:
	// NOTE: ��ȸ ��

	virtual const vector<IGameObject*>& ViewByLayer(LayerID id) const = 0;
	virtual const vector<IGameObject*>& ViewByTag(TagID id) const = 0;

	// ���� ��Ÿ ����

	virtual void SetLayer(IGameObject* obj, LayerID id) = 0;
	virtual void SetTag(IGameObject* obj, TagID id) = 0;

	virtual void SetVisible(IGameObject* obj, _bool on) = 0;
	virtual void SetPaused(IGameObject* obj, _bool on) = 0;
	virtual void SetTimeScale(IGameObject* obj, _double scale) = 0;

	// ���������� ����

	virtual void SetLayerVisible(LayerID id, _bool on) = 0;
	virtual void SetLayerPaused(LayerID id, _bool on) = 0;
	virtual void SetLayerTimeScale(LayerID id, _double scale) = 0;
	
	// ���� Ȯ��

	virtual _bool LayerVisible(LayerID id) const = 0;
	virtual _bool LayerPaused(LayerID id) const = 0;
	virtual _double LayerTimeScale(LayerID id) const = 0;
};

END