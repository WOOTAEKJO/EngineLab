#pragma once
#include "ObjectMeta.h"

BEGIN(Engine)

struct IGameObject
{
protected:
	IGameObject() = default;
	virtual ~IGameObject() = default;

public:
	virtual HRESULT InitPrototype() { return S_OK; }
	// "����(�޸� Ȯ��)���� ����� �غ񡱸� �и�
	// Ǯ���� ��Ȱ���� ���� Init()�� �ٽ� ȣ���� �ʱ�ȭ
	// ���и� bool/HRESULT�� �ǵ��� �� �־� ctor���� ���� ������ �δ��� ����

	virtual HRESULT InitInstance() { return S_OK; }
	// Ŭ�� ���� �ʱ�ȭ �Լ�

	virtual void ResetInstance() {}
	// Ǯ ���� �� ��Ÿ�� ���� ����

	virtual void OnActivate() {}; // Ȱ��ȭ ����(�� ����)
	virtual void OnDeActivate() {}; // ��Ȱ��ȭ ����(�� ����)
};

END