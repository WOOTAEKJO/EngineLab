#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;			// 문자를 저장하는 용도가 아닌 숫자를 저장하는 용도
	typedef		unsigned char				_ubyte;			// 마찬가지

	typedef		char						_char;			// 문자를 저장하는 용도
	typedef		wchar_t						_tchar;

	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;

	typedef		unsigned long long			_ulonglong;

	/* 저장용 데이터 타입. */
	typedef		XMFLOAT3X3					_float3x3;
	typedef		XMFLOAT4X4					_float4x4;

	/* 연산용 데이터 타입. */
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

	/* 저장용 데이터 타입. */
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT2					_float2;

	/* 저장용 데이터 타입. */
	typedef		XMINT2						_int2;
	typedef		XMINT3						_int3;
	typedef		XMINT4						_int4;

	/* 연산용 데이터 타입. */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;
}

#endif // Engine_Typedef_h__
