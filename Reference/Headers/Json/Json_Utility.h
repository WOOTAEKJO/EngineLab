#pragma once
#include "Engine_Defines.h"
#include "json.hpp"
using namespace nlohmann;

BEGIN(Engine)

class ENGINE_DLL CJson_Utility final
{
private:
	CJson_Utility();
	virtual	~CJson_Utility() = default;

public:
	HRESULT	Initialize();
	static HRESULT		Load_Json(const char* _strPath, json& pOut);
	static HRESULT		Save_Json(const char* _strPath, json _json);

public:
	static void			Write_Float2(json& Out_Json, const _float2& In_Float2);
	static void			Write_Float3(json& Out_Json, const _float3& In_Float3);
	static void			Write_Float4(json& Out_Json, const _float4& In_Float4);
	static void			Load_Float2(const json& In_Json, _float2& Out_Float2);
	static void			Load_Float3(const json& In_Json, _float3& Out_Float3);
	static void			Load_Float4(const json& In_Json, _float4& Out_Float4);
	static void			Load_JsonFloat4x4(const json& _Json, _float4x4& Out_Float4x4);

	static XMFLOAT4		Get_VectorFromJson(json& _json);
	static XMFLOAT4X4	Get_MatrixFromJson(json& _json);

public:
	static void			Write_Float2(Value & pValue,const _char * strName, _float2 vVec, Document & doc);
	static void			Write_Float3(Value & pValue, const _char* strName, _float3 vVec, Document& doc);
	static void			Write_Float4(Value & pValue, const _char * strName, _float4 vVec, Document & doc);

	static void			Write_uint3(Value & pValue, const _char * strName, _uint3 iValue, Document & doc);

public:
	//static wstring Complete_Path(const char* _strFileName)

};

END