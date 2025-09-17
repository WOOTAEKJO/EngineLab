#pragma once
#include "IGameObject.h"

BEGIN(Engine)

/*
	   - 서비스 레지스트리로 오브젝트 매니저를 사용하기 위한 준비
		   -> 다른 매니저와는 다르게 오브젝트 매니저는 템플릿 함수를 다수 사용하기 때문
			   -> 이는 가상함수는 템플릿으로 만들지 못하는 이유가 제일 크다.
				   -> vtable에는 "고정된 시그니처"만 올라갈 수 있다.

	   - 사용하기 위한 구조 -> "외부 API는 템플릿처럼 편하게" + "내부는 가상으로 다형"을 동시에 만족
		   - vtable에는 비템플릿 가상 함수(런타임 디스패치)를 올리고
		   - 바깥쪽에는 얇은 템플릿 래퍼를 둬서 호출 편의를 준 다음,
		   - 실제 인자는 타입 소거(AnyParams)로 넘겨 런타임에 안전히 복원한다.
*/

// --------- AnyParams ------------
struct AnyParams
{
	const void* data = nullptr;				// 실 인자 메모리 (읽기 전용)
	size_t	size = 0;						// 바이트 크기
	const type_info* type = &typeid(void);	// 원래 타입 정보(검증용)
}; /*
		타입 소거 : 아무 타입의 값을 "복사/소유 없이" 포인터 + 바이트 길이(+RTTI)로 들고 다니며,
		공통 인터페이스에 건네줄 때 쓰는 읽기 전용 바이트 뭉치이다.
   */

using CreateFn = u_ptr<IGameObject>(*)(const ObjectMeta&,AnyParams);		// 새 객체 생성만(InitInstance 및 onActivate x)
using InitFn = HRESULT(*)(IGameObject&, AnyParams);	// (오브젝트/ 파라미터) 초기화 함수 포인터. 확보 후 InitInstance 호출
using ProtoInitFn = HRESULT(*)(IGameObject&, AnyParams);	// 프로토타입 InitPrototype 호출
															// 호출함수를 의미가 섞일 수 있으므로 따로 둔다.
// - 각각 함수 포인터 멤버
// - 생성과 초기화를 나누는 이유는 기본/프로토타입/풀에서 불릴 때, 둘 다 혹은 하나만 호출하기 때문.

struct TypeBinding // 클라이언트가 등록할 바인더 묶음
{
	type_index			obj;	// 오브젝트 타입
	const type_info*	param;	// 오브젝트 파라미터

	CreateFn			create;
	InitFn				init;
	ProtoInitFn			prototypeInit;

	/*
		예) 자유 함수로 연결
			ObjUPtr MakeBullet(); // 구현은 ObjUPtr 반환
			bool InitBullet(IGameObject& o, AnyParams ap); // 내부에서 Bullet로 캐스팅해서 초기화

			TypeBinding b{
			    std::type_index{typeid(Bullet)},
			    std::type_index{typeid(BulletSpawn)},
			    &MakeBullet,   // construct
			    &InitBullet    // init
			};
			
			// 호출
			ObjUPtr up = b.construct();          // 새로 만들고
			bool ok    = b.init(*up, params);    // 파라미터로 초기화
	*/
};

// --------- ObjectService ------------
struct _declspec(novtable) ENGINE_DLL IObjectService
{
protected:
	IObjectService();
	virtual ~IObjectService();

protected: // 비템플릿 가상 함수: "타입 소거 + 런타임 디스패치"의 핵심
	virtual IGameObject* SpawnByType(type_index type, const ObjectMeta& meta, AnyParams params) = 0;
	virtual IGameObject* CloneFromPrototype(string_view key, const ObjectMeta& meta, AnyParams params = {}) = 0;
	
	virtual IGameObject* AcquireFromPool(string_view key, AnyParams params = {}) = 0;
	
	virtual HRESULT DefineSpawnRaw(const TypeBinding& b) = 0;
	virtual HRESULT DefinePrototypeInitRaw(const TypeBinding& b) = 0;
	virtual HRESULT CreatePrototypeByType(type_index type, string_view key, AnyParams params = {}) = 0;

	/*
		- 가상 + 비템플릿으로 vtable에 올라간다.
		- 인자 타입은 type_index와 AnyParams등 런타임 정보를 넘긴다.
	*/

public:
	virtual size_t Ready_Pool(string_view key, size_t targetCount) = 0;
	virtual void Release(IGameObject* obj, const string& key) = 0;

public: // NOTE: 템플릿 레퍼: 호출 편의를 제공(컴파일 타임 타입추론)
	
	/// 오브젝트 런타임 활성화
	template<class T, class P = std::monostate>
	T* Spawn(const ObjectMeta& meta, const P& p = {})
	{
		return static_cast<T*>(SpawnByType(typeid(T), meta, pack(p)));
	}
	/* NOTE:
		- P
			-> 여기서 P는 T를 생성할 때, 같이 넘기는 파라미터 값이다.
			-> P는 기본적으로 monostate타입. monostate-> 빈 타입이다.
		- pack(p)
			-> p를 AnyParams같은 타입 소거로 바꿔 런타임 디스패치로 넘긴다.
	*/

	/// 프로토타입 클론 런타임 활성화
	template<class T, class P = std::monostate>
	IGameObject* Clone(string_view key, const ObjectMeta& meta, const P& p = {})
	{
		return CloneFromPrototype(key, meta, pack(p));
	}

	/// 풀 내 오브젝트 런타임 활성화
	template<class T, class P = std::monostate>
	IGameObject* Acquire(string_view key, const P& p = {})
	{
		return AcquireFromPool(key, pack(p));
	}

	/// 규칙 및 콜백 등록
	template<class T, class P = monostate> // 템플릿 헬퍼
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

			if constexpr (is_same_v<P, monostate>) // 파라미터가 없을 때
			{
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitInstance();
				return obj.InitInstance();
			}
			else // 파라미터가 있을 때
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitInstance(p ? *p : P{});
				return E_FAIL;
			}
		};

		return DefineSpawnRaw(bind);
	}

	/// 프로토타입 초기화 규칙 및 콜백 등록
	template<class T, class P = monostate>
	HRESULT DefinePrototypeInit()
	{
		TypeBinding bind;
		bind.obj = typeid(T);
		bind.param = typeid(P);

		bind.prototypeInit = [](IGameObject& obj, AnyParams ap)
		{
			if (ap.type != &typeid(P)) return E_FAIL;

			if constexpr (is_same_v<P, monostate>) // 파라미터가 없을 때
			{
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitPrototype();
				return obj.InitPrototype();
			}
			else // 파라미터가 있을 때
			{
				const P* p = ap.size ? static_cast<const P*>(ap.data) : nullptr;
				if (auto* t = dynamic_cast<T*>(&obj))
					return t->InitPrototype(p ? *p : P{});
				return E_FAIL;
			}
		};

		return DefinePrototypeInit(bind);
	}

	/// 프로토타입 실체를 생성
	template<class T>
	HRESULT CreatePrototype(string_view key)
	{
		return CreatePrototypeByType(typeid(T), key);
	}

public:
	// NOTE: 조회 뷰

	virtual const vector<IGameObject*>& ViewByLayer(LayerID id) const = 0;
	virtual const vector<IGameObject*>& ViewByTag(TagID id) const = 0;

	// 개별 메타 제어

	virtual void SetLayer(IGameObject* obj, LayerID id) = 0;
	virtual void SetTag(IGameObject* obj, TagID id) = 0;

	virtual void SetVisible(IGameObject* obj, _bool on) = 0;
	virtual void SetPaused(IGameObject* obj, _bool on) = 0;
	virtual void SetTimeScale(IGameObject* obj, _double scale) = 0;

	// 전역적으로 수정

	virtual void SetLayerVisible(LayerID id, _bool on) = 0;
	virtual void SetLayerPaused(LayerID id, _bool on) = 0;
	virtual void SetLayerTimeScale(LayerID id, _double scale) = 0;
	
	// 상태 확인

	virtual _bool LayerVisible(LayerID id) const = 0;
	virtual _bool LayerPaused(LayerID id) const = 0;
	virtual _double LayerTimeScale(LayerID id) const = 0;
	
private: // AnyParams로 타입/주소/크기를 함께 포장한다.
	static AnyParams pack(const std::monostate&) { return { nullptr,0,&typeid(void) }; } // 빈통 포장

	template<class P>
	static AnyParams pack(const P& p) { return { &p,sizeof(p),&typeid(p) }; }; // 타입 P에 대한 포장

};

END