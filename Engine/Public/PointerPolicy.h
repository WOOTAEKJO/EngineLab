#pragma once
#include <memory>
#include <type_traits>
#include <utility>

// T: ������ ���������� �ִ� Ÿ��
	// Args...: T�� �����ڿ� �Ѱ��� ���ڵ�
//template<typename T, typename... Args>
//unique_ptr<T> make_unique_enabler(Args&&... args)
//{
//	//1) Enabler: T�� public ��ӹ޴� ���� �Ļ� Ŭ����
//	struct Enabler : public T {
//		// ���޹��� Args...�� T�� �����ڿ� ������
//		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
//	};
//
//	// 2) new Enabler, ��ȯ �� unique_ptr<T>�� ��ȯ
//	return unique_ptr<T>(new Enabler(forward<Args>(args)...));
//
//	/*
//		Enabler�� ���ø� �Լ� �ȿ� ���ǵ� ���� Ŭ�����̹Ƿ�,
//		T�� protected (Ȥ�� private�� friend) �����ڿ��� ������ �� �ֽ��ϴ�.
//		T�� �Ļ� Ŭ������ Enabler�� ����� ����(Protected) ����.
//
//		��ȯ Ÿ���� std::unique_ptr<T> �̹Ƿ�, ȣ���ڴ� T ��üó�� �ٷ� �� �ֽ��ϴ�.
//	*/
//}

template<class Static>
struct erased_deleter
{
	using Fn = void(*)(Static*) noexcept; // noexcept : �����Ϸ����� ���ܸ� ������ �ʴ´ٰ� ����. ����ϸ� ����ȭ ����.
	Fn fn; // �Լ� ������ Ÿ�� ��Ī

	void operator()(Static* p) const noexcept
	{
		if (p) fn(p);
	} // unique_ptr ������ ������

	template<class Enabler>
	static void del_as_enabler(Static* p) noexcept
	{
		delete static_cast<Enabler*>(p);
	} // ���� ���� ����

}; // �ַ� �Ҹ��ڰ� protected/private�� ���̽��� unique_ptr�� �����ϰ� �����Ϸ��� �� �� ���.

template<typename T, typename...Args>
unique_ptr<T, erased_deleter<T>> make_unique_enabler(Args&&... args)
{
	static_assert(!std::is_final_v<T>, "T�� �����Դϴ�. Enabler�� �Ļ��� �� �����ϴ�.");

	struct Enabler : public T
	{
		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	};

	using Del = erased_deleter<T>;

	return unique_ptr<T, Del>(static_cast<T*>(new Enabler(forward<Args>(args)...)),
		Del{ &Del::template del_as_enabler<Enabler> });
}

template<typename base, typename T, typename...Args>
unique_ptr<base, erased_deleter<base>> make_unique_enabler(Args&&... args)
{
	static_assert(std::is_base_of_v<base, T>, "T�� Base���� �Ļ��Ǿ�� �մϴ�.");
	static_assert(!std::is_final_v<T>, "T�� �����Դϴ�. Enabler�� �Ļ��� �� �����ϴ�.");

	struct Enabler : public T
	{
		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	};

	using Del = erased_deleter<base>;

	return unique_ptr<base, Del>(
		static_cast<base*>(new Enabler(forward<Args>(args)...)),
		Del{ &Del::template del_as_enabler<Enabler> });
}
/*
	- base ������ �ϳ� �� ����� ����.
		1. ������ ���� Ÿ���� �ٸ�
		2. ������ Ÿ�Ե� �ٸ�
			1�� ������ ��ĳ�������� �� ��ȯ�� ����������, 2���� ���� �ٸ� Ÿ���̶� ��ȯ�� �Ұ����ϴ�.

		- �Ŵ������� ������ ����ϴ� ���������� �����̳ʴ� ���� base ���·� ������ �����Ѵ�.
			�� ���� ��, ������ ���� Ÿ�԰� ������ Ÿ���� ���ƾ� �ϴµ�, �⺻���� �Լ����� ����ϸ� ������ Ÿ���� ���Ƶ� ������ Ÿ���� �ٸ���.
			�׷��� ���ʿ� �������� ������ ��, ������ Ÿ�԰� ������ Ÿ���� ���� ����� �ذ�ȴ�.
		��)
			�Ŵ��� Ŭ���� ������ �����̳� -> unordered_map<type_index, unique_ptr<IManager,erased_deleter<IManager>>> m;
			������ �Ŵ��� -> unique_ptr<Renderer,erased_deleter<Renderer>>
			����ȯ�� �ص� -> unique_ptr<IManager,erased_deleter<Renderer>> �̱� ������ ���� �����̳ʿ� ���� ��ü�ʹ� �ٸ�.
			�׷��� ������ ó������ unique_ptr<IManager,erased_deleter<IManager>>�� ����� �����ϴ°� ����.
				���⼭ IMnager�� base������ Ŭ����. Ư�� �Ŵ����� �ٿ�ĳ�������� ����ϸ� �ȴ�.
*/

/// T�� protected ��������ڸ� �Ļ� Enabler�� ��ȸ
template<class base, class T>
unique_ptr<base, erased_deleter<base>> clone_unique_enabler(const T& src)
{
	static_assert(std::is_base_of_v<base, T>, "T�� Base���� �Ļ��Ǿ�� �մϴ�.");
	static_assert(!std::is_final_v<T>, "T�� �����Դϴ�. Enabler�� �Ļ��� �� �����ϴ�.");

	struct Enabler : T
	{
		Enabler(const T& s) : T(s) {} // T�� ��������ڰ� protected���� �����ϴ�. private�� �Ұ�
	};

	using Del = erased_deleter<base>;

	T* raw = new Enabler(src);
	return { static_cast<base*>(raw),Del{ &Del::template del_as_enabler<Enabler> } };
}

template<typename T, typename... Args>
shared_ptr<T> make_shared_enabler(Args&&... args)
{
	struct Enabler : public T
	{
		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
	};

	return shared_ptr<T>(new Enabler(forward<Args>(args)...));
}

// --------- �Ϲ� ---------------
struct UniquePolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return unique_ptr<U>(new U(forward<A>(a)...));
	}
};

struct SharedPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return shared_ptr<U>(new U(forward<A>(a)...));
	}
};

// ------- Enabler ------------

struct UniqueEnablerPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return make_unique_enabler<U>(forward<A>(a)...);
	}
}; // �⺻ ����

template<typename base>
struct UniqueEnablerAsPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return make_unique_enabler<base, U>(forward<A>(a)...);
	}
}; // base ����:: base���·� ������ �� ���. �Ŵ��� Ŭ���� ���

struct SharedEnablerPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return make_shared_enabler<U>(forward<A>(a)...);
	}
};