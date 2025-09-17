#ifndef Engine_Macro_h__
#define Engine_Macro_h__


#ifndef			MSG_BOX
#define			MSG_BOX(_message)		MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK) // 문자열 리터럴용

#define			MSG_BOX_VAR(p)		MessageBox(nullptr, p, L"System Message", MB_OK)// 변수 -> const TCHAR* 용

#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__asm { int 3 };return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__asm { int 3 };return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__asm { int 3 };}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__asm { int 3 };return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __asm { int 3 };return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __asm { int 3 };return _return;}



#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

// ----------- 추가 --------------------

#define CREATE_INSTANCE(CLASSNAME)								\
	template<typename CLASSNAME>								\
	friend shared_ptr<CLASSNAME> Engine::Create_Instance();

#define SINGLETON_REGISTRATION(CLASSNAME)									\
	template<typename CLASSNAME>								\
	friend auto Engine::GetInstance<CLASSNAME>();

#define SAFETY(PTR)												\
	return Safety(PTR);

#define RETURN_EFAIL {assert(false); return E_FAIL;}

// ------- 스마트 포인터 enabler ----------------

#define MAKE_UNIQUE(T, ...) make_unique_enabler<T>(__VA_ARGS__)

#define MAKE_SHARED(T, ...) make_shared_enabler<T>(__VA_ARGS__)

// ------ shared_ptr + enabler Singleton ---------- C++ 17 이상 버전만

#define DECLARE_SHARED_SINGLETON(ClassName)						\
public:															\
	static shared_ptr<ClassName> Get_Instance() {				\
	call_once(m_onceFlag, [](){									\
																\
		m_pInstance = make_shared_enabler<ClassName>();			\
	});															\
	return m_pInstance;											\
	}															\
protected:														\
	ClassName();												\
	~ClassName();												\
private:														\
	ClassName(const ClassName&) = delete;						\
	ClassName& operator=(const ClassName&) = delete;			\
	inline static shared_ptr<ClassName> m_pInstance = nullptr;	\
	inline static once_flag			 m_onceFlag;			

// -------- CoustomDeleter ------------ 사용X
#define ENGINE_HIDE_DTOR_DEFAULT_DELETE(Type) \
protected:                                     \
    ~Type() = default;                         \
    friend struct std::default_delete<Type>;

//#define	COMPONENT_BUFFER		TEXT("Com_Buffer")
//#define	COMPONENT_MODEL			TEXT("Com_Model")
//#define	COMPONENT_SHADER		TEXT("Com_Shader")
//#define	COMPONENT_STATEMACHINE	TEXT("Com_Statemachine")
//#define	COMPONENT_TRANSFORM		TEXT("Com_Transform")
//#define	COMPONENT_TEXTURE		TEXT("Com_Texture")
//#define	COMPONENT_AI			TEXT("Com_AI")
//#define	COMPONENT_NAVIGATION	TEXT("Com_Navigation")
//#define	COMPONENT_RIGIDBODY		TEXT("Com_RigidBody")

//#define	COMPONENT_BUFFER_CELL	typeid(CVIBuffer_Cell).name()
//#define	COMPONENT_MODEL			typeid(CModel).name()
//#define	COMPONENT_SHADER		typeid(CShader).name()
//#define	COMPONENT_STATEMACHINE	typeid(CStateMachine).name()
//#define	COMPONENT_TRANSFORM		typeid(CTransform).name()
//#define	COMPONENT_TEXTURE		typeid(CTexture).name()
//#define	COMPONENT_AI			typeid(CAICom).name()
//#define	COMPONENT_NAVIGATION	typeid(CNavigation).name()
//#define	COMPONENT_RIGIDBODY		typeid(CRigidBody).name()

#endif // Engine_Macro_h__
