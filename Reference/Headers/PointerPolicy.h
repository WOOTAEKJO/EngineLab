#pragma once
#include <memory>
#include <type_traits>
#include <utility>

// T: 생성자 접근제한이 있는 타입
	// Args...: T의 생성자에 넘겨줄 인자들
//template<typename T, typename... Args>
//unique_ptr<T> make_unique_enabler(Args&&... args)
//{
//	//1) Enabler: T를 public 상속받는 로컬 파생 클래스
//	struct Enabler : public T {
//		// 전달받은 Args...를 T의 생성자에 포워딩
//		Enabler(Args&&... a) : T(forward<Args>(a)...) {}
//	};
//
//	// 2) new Enabler, 반환 시 unique_ptr<T>로 변환
//	return unique_ptr<T>(new Enabler(forward<Args>(args)...));
//
//	/*
//		Enabler는 템플릿 함수 안에 정의된 로컬 클래스이므로,
//		T의 protected (혹은 private에 friend) 생성자에도 접근할 수 있습니다.
//		T의 파생 클래스인 Enabler를 만들어 접근(Protected) 가능.
//
//		반환 타입은 std::unique_ptr<T> 이므로, 호출자는 T 객체처럼 다룰 수 있습니다.
//	*/
//}

template<class Static>
struct erased_deleter
{
	using Fn = void(*)(Static*) noexcept; // noexcept : 컴파일러에게 예외를 던지지 않는다고 보장. 사용하면 최적화 가능.
	Fn fn; // 함수 포인터 타입 별칭

	void operator()(Static* p) const noexcept
	{
		if (p) fn(p);
	} // unique_ptr 삭제자 연산자

	template<class Enabler>
	static void del_as_enabler(Static* p) noexcept
	{
		delete static_cast<Enabler*>(p);
	} // 실제 삭제 구현

}; // 주로 소멸자가 protected/private인 베이스를 unique_ptr로 안전하게 관리하려고 할 때 사용.

template<typename T, typename...Args>
unique_ptr<T, erased_deleter<T>> make_unique_enabler(Args&&... args)
{
	static_assert(!std::is_final_v<T>, "T는 최종입니다. Enabler를 파생할 수 없습니다.");

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
	static_assert(std::is_base_of_v<base, T>, "T는 Base에서 파생되어야 합니다.");
	static_assert(!std::is_final_v<T>, "T는 최종입니다. Enabler를 파생할 수 없습니다.");

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
	- base 버전을 하나 더 만드는 이유.
		1. 포인터 정적 타입이 다름
		2. 딜리터 타입도 다름
			1번 이유는 업캐스팅으로 형 변환이 가능하지만, 2번은 서로 다른 타입이라서 변환이 불가능하다.

		- 매니저같은 곳에서 사용하는 관리형태의 컨테이너는 보통 base 형태로 가져서 관리한다.
			이 말은 즉, 포인터 정적 타입과 딜리터 타입이 같아야 하는데, 기본적인 함수만을 사용하면 포인터 타입은 같아도 딜리터 타입은 다르다.
			그러면 애초에 빌더에서 생성할 때, 포인터 타입과 딜리터 타입을 같게 만들면 해결된다.
		예)
			매니저 클래스 관리형 컨테이너 -> unordered_map<type_index, unique_ptr<IManager,erased_deleter<IManager>>> m;
			렌더러 매니저 -> unique_ptr<Renderer,erased_deleter<Renderer>>
			형변환을 해도 -> unique_ptr<IManager,erased_deleter<Renderer>> 이기 때문에 관리 컨테이너에 들어가는 객체와는 다름.
			그렇게 때문에 처음부터 unique_ptr<IManager,erased_deleter<IManager>>로 만들어 관리하는게 좋다.
				여기서 IMnager가 base형태의 클래스. 특정 매니저는 다운캐스팅으로 사용하면 된다.
*/

/// T의 protected 복사생성자를 파생 Enabler로 우회
template<class base, class T>
unique_ptr<base, erased_deleter<base>> clone_unique_enabler(const T& src)
{
	static_assert(std::is_base_of_v<base, T>, "T는 Base에서 파생되어야 합니다.");
	static_assert(!std::is_final_v<T>, "T는 최종입니다. Enabler를 파생할 수 없습니다.");

	struct Enabler : T
	{
		Enabler(const T& s) : T(s) {} // T의 복사생성자가 protected여야 가능하다. private은 불가
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

// --------- 일반 ---------------
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
}; // 기본 버전

template<typename base>
struct UniqueEnablerAsPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return make_unique_enabler<base, U>(forward<A>(a)...);
	}
}; // base 버전:: base형태로 관리할 때 사용. 매니저 클래스 등등

struct SharedEnablerPolicy
{
	template<class U, class...A>
	static auto create(A&&... a)
	{
		return make_shared_enabler<U>(forward<A>(a)...);
	}
};