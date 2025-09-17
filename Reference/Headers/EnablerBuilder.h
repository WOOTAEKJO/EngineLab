#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

template<class T, class... CtorArgs>
class EnablerBuilder
{
public:
	explicit EnablerBuilder(CtorArgs&&... args)
		: m_ctorArgs(forward<CtorArgs>(args)...) {}

	shared_ptr<T> build() {
		return apply([](auto&&... a) {
			return make_shared_enabler<T>(forward<decltype(a)>(a)...);
			}, m_ctorArgs);
	}

	template<class... InitArgs>
	shared_ptr<T> init(InitArgs&&... args)
	{
		auto ptr = build();
		ptr->Init(forward<InitArgs>(args)...);
		return ptr;
	}

private:
	tuple<decay_t<CtorArgs>...> m_ctorArgs;
};
END