#pragma once

#pragma warning(disable : 4251)

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

}

#include "../Fmod/core/fmod.h"
#include "../Fmod/core/fmod.hpp"
#include "../Fmod/core/fmod_errors.h"
#include "../Fmod/studio/fmod_studio.h"
#include "../Fmod/studio/fmod_studio.hpp"
#include "../Fmod/core/fmod_common.h"

//#include <DirectXTK/PrimitiveBatch.h>
//#include <DirectXTK/Effects.h>
//#include <DirectXTK/VertexTypes.h>
//#include <DirectXTK/SpriteBatch.h>
//#include <DirectXTK/SpriteFont.h>
//#include "../Public/DirectXTK/PrimitiveBatch.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <random>





#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK/SimpleMath.h"

#include <d3dcompiler.h>
using namespace DirectX;

// 스탠다드 라이브러리
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <queue>
#include <tuple>
#include <array>

#include <typeinfo>
#include <typeindex>
#include <functional>
#include <string>
#include <string_view>
#include <cassert>
#include <memory>
#include <limits>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <utility>
#include <wrl/client.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

using Microsoft::WRL::ComPtr; // comptr을 사용하기 위함

namespace Engine
{
	static const wchar_t* g_pTransformTag = TEXT("Com_Transform");
}



#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#include "StringConvert.h" // builder보다 앞에 포함되어 있어야 한다;;

#include "BuilderMacro.h"

using namespace Engine;



#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include <locale>
#include <codecvt>

#include "commdlg.h"
#include "shlwapi.h"
#include <fstream>
using namespace rapidjson;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#else
#define DBG_NEW new
//#endif

#endif // _DEBUG




