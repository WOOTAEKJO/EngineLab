#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Typedef.h"

namespace Engine
{
	// ���ø��� ����� ������������ �ڷ����� ���������� ���� ��
	// ����� �ν��Ͻ�ȭ �ϱ� ���Ͽ� �����δ� Ʋ

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
#pragma region ���� ��Ŀ����
	template<typename T>
	shared_ptr<T> Create_Instance()
	{
		auto deleter = [](T* pInstance)
		{
			if (is_base_of<class CBase, T>::value)
				pInstance->Free();
			delete pInstance;
		};// �̰� ���� ���� �ٸ� ���ø� �Լ��� ���� �� ������, �׷��� �̰� ���� friend Ű���带 Ŭ�������� �ٿ�����;;

		shared_ptr<T> pInstance(new T(), deleter);

		if (FAILED(pInstance->Initialize()))
		{
			assert(false);
			return nullptr;
		}

		return pInstance;
	} // Ŭ�������� �� �Լ��� friend Ű���带 �ٿ��� ������ private�� ���� �����ϰ� �����.(������/ �Ҹ���)

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
		cout << ss + " �̱��� ���� " << endl;

		return unique_ptr<T, decltype(deleter)>(new T(), deleter);
	}// �̱��� ��ü ���� �Ǵ� ȣ��
#pragma endregion // ���� Ŭ������ ��ü

#pragma region ����Ʈ ������ enabler

	//// T: ������ ���������� �ִ� Ÿ��
	//// Args...: T�� �����ڿ� �Ѱ��� ���ڵ�
	//template<typename T, typename... Args>
	//unique_ptr<T> make_unique_enabler(Args&&... args)
	//{
	//	//1) Enabler: T�� public ��ӹ޴� ���� �Ļ� Ŭ����	
	//	struct Enabler : public T {
	//		// ���޹��� Args...�� T�� �����ڿ� ������
	//		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	//	};

	//	// 2) new Enabler, ��ȯ �� unique_ptr<T>�� ��ȯ
	//	return unique_ptr<T>(new Enabler(forward<Args>(args)...));

	//	/*
	//		Enabler�� ���ø� �Լ� �ȿ� ���ǵ� ���� Ŭ�����̹Ƿ�,
	//		T�� protected (Ȥ�� private�� friend) �����ڿ��� ������ �� �ֽ��ϴ�.

	//		��ȯ Ÿ���� std::unique_ptr<T> �̹Ƿ�, ȣ���ڴ� T ��üó�� �ٷ� �� �ֽ��ϴ�.
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
