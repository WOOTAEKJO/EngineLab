#pragma once
#include <memory>

template<class T>
struct HideDeleter
{
protected:
	~HideDeleter() = default;
	friend struct std::default_delete<T>;
};