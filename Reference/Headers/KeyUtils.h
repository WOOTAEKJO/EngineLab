#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

// --------- AnyParams ------------
struct AnyParams
{
	const void* data = nullptr;				// �� ���� �޸� (�б� ����)
	size_t	size = 0;						// ����Ʈ ũ��
	type_index type = typeid(void);
}; /*
		Ÿ�� �Ұ� : �ƹ� Ÿ���� ���� "����/���� ����" ������ + ����Ʈ ����(+RTTI)�� ��� �ٴϸ�,
		���� �������̽��� �ǳ��� �� ���� �б� ���� ����Ʈ ��ġ�̴�.
   */

template<class P>
using D = remove_cv_t<remove_reference_t<P>>; // const T, volatile T, const volatile T���� cv ���š� �ᱹ �ǻ� Ÿ�� T

/// ���ø� Ÿ�� P�� ���۷���/const/volatile ������ �� �Ⱦ ������ Ÿ�ԡ��� std::type_index**�� �����ִ� ��ƿ
template<class P>
inline type_index type_of() { return typeid(D<P>); }

struct Key
{
	type_index obj;			// ������ "������Ʈ Ÿ��"�� typeid(T)
	type_index param;		// obj�� ���� type_index�� �����ϸ� �� ������
	//const type_info* param; // �߰� �Ķ���� Ÿ���� typeid(p) �ּ� 
	bool operator==(const Key& o) const { return obj == o.obj && param == o.param; }
};/*
	- obj�� std::type_index�� ���� �� typeid(T) ��/�ؽð� ����.
	- param�� �Ķ���� Ÿ��(��: BulletParams)�� type_info* �����͸� �״�� ����.
		-> ��, ���� T�� P�� �ٸ��� �ٸ� Key�� �ȴ�.
  */

struct KeyHash
{
	size_t operator()(const Key& k) const noexcept
	{
		//return hash<type_index>{}(k.obj) ^ (reinterpret_cast<uintptr_t>(k.param) >> 3);
		// �ؽ�: obj�� �ؽÿ� param ������ ���� ����. >> 3�� ���� 3��Ʈ�� �о� �ణ ���� Ʈ��.

		size_t h1 = hash<type_index>{}(k.obj);
		size_t h2 = hash<type_index>{}(k.param);
		return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
		// ����� hash-combine�� ���.
	}
};

// ------- ������Ʈ �Ŵ������� ��� -----------
template<class T, class P>
inline Key SpawnKey() { return { typeid(T),type_of<P>() }; }
inline Key SpawnKeyErased(type_index t, type_index p) { return { t,p }; }
// ������ (T,P)

template<class T>
inline Key CreateKey() { return { typeid(T),typeid(monostate) }; }
inline Key CreateKeyErased(type_index t) { return { t,typeid(monostate) }; }
// ������ �׻� (T,monostate) Ű �ϳ��� ����

// AnyParams�� Ÿ��/�ּ�/ũ�⸦ �Բ� �����Ѵ�.
inline AnyParams pack(const std::monostate) { return { nullptr,0,typeid(monostate) }; } // ���� ����

template<class P>
inline AnyParams pack(const P& p)
{
	return { addressof(p),sizeof(D<P>),typeid(D<P>) };
}
// Ÿ�� P�� ���� ����


END