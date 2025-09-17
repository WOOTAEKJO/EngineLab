#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Typedef.h"

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)

			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}
#pragma region 생성 메커니즘
	template<typename T>
	shared_ptr<T> Create_Instance()
	{
		auto deleter = [](T* pInstance)
		{
			if (is_base_of<class CBase, T>::value)
				pInstance->Free();
			delete pInstance;
		};// 이걸 밖을 빼서 다른 템플릿 함수로 만들 수 있지만, 그러면 이것 또한 friend 키워드를 클래스마다 붙여야함;;

		shared_ptr<T> pInstance(new T(), deleter);

		if (FAILED(pInstance->Initialize()))
		{
			assert(false);
			return nullptr;
		}

		return pInstance;
	} // 클래스마다 이 함수를 friend 키워드를 붙였기 때문에 private도 접근 가능하게 해줬다.(생성자/ 소멸자)

	template<typename T>
	auto GetInstance()
	{
		auto deleter = [](T* pInstance)
		{
			if (is_base_of<class CBase, T>::value)
				pInstance->Free();
			delete pInstance;
		};
		string ss = typeid(T).name();
		cout << ss + " 싱글톤 생성 " << endl;

		return unique_ptr<T, decltype(deleter)>(new T(), deleter);
	}// 싱글톤 객체 생성 또는 호출
#pragma endregion // 생성 클래스로 대체

#pragma region 스마트 포인터 enabler

	//// T: 생성자 접근제한이 있는 타입
	//// Args...: T의 생성자에 넘겨줄 인자들
	//template<typename T, typename... Args>
	//unique_ptr<T> make_unique_enabler(Args&&... args)
	//{
	//	//1) Enabler: T를 public 상속받는 로컬 파생 클래스	
	//	struct Enabler : public T {
	//		// 전달받은 Args...를 T의 생성자에 포워딩
	//		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	//	};

	//	// 2) new Enabler, 반환 시 unique_ptr<T>로 변환
	//	return unique_ptr<T>(new Enabler(forward<Args>(args)...));

	//	/*
	//		Enabler는 템플릿 함수 안에 정의된 로컬 클래스이므로,
	//		T의 protected (혹은 private에 friend) 생성자에도 접근할 수 있습니다.

	//		반환 타입은 std::unique_ptr<T> 이므로, 호출자는 T 객체처럼 다룰 수 있습니다.
	//	*/
	//}

	//template<typename T, typename... Args>
	//unique_ptr<T> make_unique_enabler_init(Args&&... args)
	//{
	//	auto ptr = make_unique_enabler<T>(forward<Args>(args)...);
	//	ptr->Init();
	//	return ptr;
	//}

	//template<typename T, typename... Args>
	//shared_ptr<T> make_shared_enabler(Args&&... args)
	//{
	//	struct Enabler : public T
	//	{
	//		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	//	};

	//	return shared_ptr<T>(new Enabler(forward<Args>(args)...));
	//}

	//template<typename T, typename... Args>
	//shared_ptr<T> make_shared_enabler_init(Args&&... args)
	//{
	//	auto ptr = make_shared_enabler<T>(forward<Args>(args)...);
	//	ptr->Init();
	//	return ptr;
	//}
	

#pragma endregion
	
	template<typename T>
	HRESULT	Safety(T* pInst)
	{
		if (pInst == nullptr)
			return E_FAIL;
	}

	template<typename T>
	void Swap(T& lhs, T& rhs)
	{
		T temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	template<typename T>
	T Random(initializer_list<T> _il)
	{
		assert(0 < _il.size());

		auto it = _il.begin();
		std::advance(it, rand() % _il.size());

		return *it;
	}

	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, T>::type
		Clamp(T low, T high, T value)
	{
		return min(max(value, low), high);
	}
}

#endif // Engine_Function_h__
