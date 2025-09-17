#pragma once
#include <string>
#include <Windows.h>

BEGIN(Engine)

inline std::wstring ToWide(std::string& s)
{
	if (s.empty()) return {};

	int len = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);

	std::wstring w(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), len);

	return w;
}

END