#pragma once
#include "Engine_Defines.h"
#include "PointerPolicy.h"

BEGIN(Engine)

template<class PtrPolicy, class T, class... CtorArgs>
class MagicBuilder
{
public:
	explicit MagicBuilder(CtorArgs&&... args)
		: m_ctorArgs(forward<CtorArgs>(args)...) {}

	auto build() {
		return apply([](auto&&... a) { 
			return PtrPolicy::template create<T>(forward<decltype(a)>(a)...);
			}, m_ctorArgs);
	}

	template<class... InitArgs>
	auto Init(InitArgs&&... args)
	{
		auto ptr = build();
		if (FAILED(ptr->Init(forward<InitArgs>(args)...)))
		{
			string str = typeid(T).name();
			wstring wstr = L"Failed to Created :";
			wstr += ToWide(str);

			MSG_BOX_VAR(wstr.c_str());
			return decltype(ptr){}; // 실패 시 빈 smart-ptr 반환
		}
		return ptr;
	}

private:
	tuple<decay_t<CtorArgs>...> m_ctorArgs; // 생성자 인자 보관
};

// ------- 헬퍼 함수들 ------------

template<class T, class... A>
inline auto make_unique_builder(A&&... a){
	return MagicBuilder<UniquePolicy, T, A...>(forward<A>(a)...);
}

template<class T, class... A>
inline auto make_unique_enabler_builder(A&&... a) {
	return MagicBuilder<UniqueEnablerPolicy, T, A...>(forward<A>(a)...);
} // 기본 + 딜리터

template<class base,class T, class... A>
inline auto make_unique_enabler_as_builder(A&&... a) {
	return MagicBuilder<UniqueEnablerAsPolicy<base>, T, A...>(forward<A>(a)...);
} // 베이스 + 딜리터

template<class T, class... A>
inline auto make_shared_builder(A&&... a) {
	return MagicBuilder<SharedPolicy, T, A...>(forward<A>(a)...);
}

template<class T, class... A>
inline auto make_shared_enabler_builder(A&&... a) {
	return MagicBuilder<SharedEnablerPolicy, T, A...>(forward<A>(a)...);
}

END