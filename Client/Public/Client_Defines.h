#pragma once
namespace Client
{
	static const unsigned int	g_iWinSizeX = 1280;
	static const unsigned int	g_iWinSizeY = 720;

	enum LEVEL {
		LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_LOGO, LEVEL_END
	};
}

//#include "../Public/ImGuizmo/ImGuizmo.h"

#include "Engine_Defines.h"

using namespace Client;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;