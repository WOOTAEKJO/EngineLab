#pragma once
#include "IGameObject.h"

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

// --------- AnyParams ------------
struct AnyParams
{
	const void* data = nullptr;				// �� ���� �޸� (�б� ����)
	size_t	size = 0;						// ����Ʈ ũ��
	const type_info* type = &typeid(void);	// ���� Ÿ�� ����(������)
}; /*
		Ÿ�� �Ұ� : �ƹ� Ÿ���� ���� "����/���� ����" ������ + ����Ʈ ����(+RTTI)�� ��� �ٴϸ�,
		���� �������̽��� �ǳ��� �� ���� �б� ���� ����Ʈ ��ġ�̴�.
   */

using CreateFn = u_ptr<IGameObject>(*)(const ObjectMeta&,AnyParams);		// �� ��ü ������(InitInstance �� onActivate x)
using InitFn = HRESULT(*)(IGameObject&, AnyParams);	// (������Ʈ/ �Ķ����) �ʱ�ȭ �Լ� ������. Ȯ�� �� InitInstance ȣ��
using ProtoInitFn = HRESULT(*)(IGameObject&, AnyParams);	// ������Ÿ�� InitPrototype ȣ��
															// ȣ���Լ��� �ǹ̰� ���� �� �����Ƿ� ���� �д�.
// - ���� �Լ� ������ ���
// - ������ �ʱ�ȭ�� ������ ������ �⺻/������Ÿ��/Ǯ���� �Ҹ� ��, �� �� Ȥ�� �ϳ��� ȣ���ϱ� ����.

struct TypeBinding // Ŭ���̾�Ʈ�� ����� ���δ� ����
{
	type_index			obj;	// ������Ʈ Ÿ��
	const type_info*	param;	// ������Ʈ �Ķ����

	CreateFn			create;
	InitFn				init;
	ProtoInitFn			prototypeInit;

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
	
	virtual HRESULT DefineSpawnRaw(const TypeBinding& b) = 0;
	virtual HRESULT DefinePrototypeInitRaw(const TypeBinding& b) = 0;
	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) = 0;

	/*
		- ���� + �����ø����� vtable�� �ö󰣴�.
		- ���� Ÿ���� type_index�� AnyParams�� ��Ÿ�� ������ �ѱ��.
	*/

public:
	virtual size_t Ready_Pool(string_view key, size_t targetCount) = 0;
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
	template<class T, class P = std::monostate>
	IGameObject* Clone(string_view key, const ObjectMeta& meta, const P& p = {})
	{
		return CloneFromPrototype(key, meta, pack(p));
	}

	/// Ǯ �� ������Ʈ ��Ÿ�� Ȱ��ȭ
	template<class T, class P = std::monostate>
	IGameObject* Acquire(string_view key, const P& p = {})
	{
		return AcquireFromPool(key, pack(p));
	}

	/// ��Ģ �� �ݹ� ���
	template<class T, class P = monostate> // ���ø� ����
	HRESULT DefineSpawn()
	{
		TypeBinding bind;
		bind.obj = typeid(T);
		bind.param = typeid(P);

		bind.create = [](const ObjectMeta& meta, AnyParams ap) -> u_ptr<IGameObject>
		{
			(void)meta;

			if (ap.type != &typeid(P)) return {};
			return make_unique_enabler_as_builder<IGameObject, T>().build();
		};

		bind.init = [](IGameObject& obj, AnyParams ap) ->HRESULT
		{
			if (ap.type != &typeid(P)) return E_FAIL;

			if constexpr (is_same_v<P, monostate>) // �Ķ���Ͱ� ���� ��
			{
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitInstance();
				return obj.InitInstance();
			}
			else // �Ķ���Ͱ� ���� ��
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitInstance(p ? *p : P{});
				return E_FAIL;
			}
		};

		return DefineSpawnRaw(bind);
	}

	/// ������Ÿ�� �ʱ�ȭ ��Ģ �� �ݹ� ���
	template<class T, class P = monostate>
	HRESULT DefinePrototypeInit()
	{
		TypeBinding bind;
		bind.obj = typeid(T);
		bind.param = typeid(P);

		bind.prototypeInit = [](IGameObject& obj, AnyParams ap)
		{
			if (ap.type != &typeid(P)) return E_FAIL;

			if constexpr (is_same_v<P, monostate>) // �Ķ���Ͱ� ���� ��
			{
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitPrototype();
				return obj.InitPrototype();
			}
			else // �Ķ���Ͱ� ���� ��
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitPrototype(p ? *p : P{});
				return E_FAIL;
			}
		};

		return DefinePrototypeInit(bind);
	}

	/// ������Ÿ�� ��ü�� ����
	template<class T>
	HRESULT CreatePrototype(string_view key)
	{
		return CreatePrototypeByType(typeid(T), key);
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
	
private: // AnyParams�� Ÿ��/�ּ�/ũ�⸦ �Բ� �����Ѵ�.
	static AnyParams pack(const std::monostate&) { return { nullptr,0,&typeid(void) }; } // ���� ����

	template<class P>
	static AnyParams pack(const P& p) { return { &p,sizeof(p),&typeid(p) }; }; // Ÿ�� P�� ���� ����

};

END