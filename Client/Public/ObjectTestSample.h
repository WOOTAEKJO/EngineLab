#pragma once
#include "../Public/OB_Test_Monster.h"

struct OTBulletSpawn { float px, py, pz, dx, dy, dz; int team; };
struct OTBulletProtu { string mesh; string material; };

class OB_Test_Bullet : public IGameObject
{
protected:
	OB_Test_Bullet() = default;
	OB_Test_Bullet(const OB_Test_Bullet&) = default;
	~OB_Test_Bullet() = default;

public:
	virtual HRESULT InitPrototype(OTBulletProtu p);
	virtual HRESULT InitInstance(OTBulletSpawn s);
	virtual void ResetInstance();

private:
	OTBulletSpawn m_spawn;
	OTBulletProtu m_proto;
};

// ------ �׽�Ʈ �Լ� -----------

namespace OBT
{
	void SpawnFromType_NOPrototype(); // Ÿ������ �ٷ� ���� -> ������Ÿ�� ����
	void SpawnFromPrototype(); // ������Ÿ�Կ��� ����
	void PrewarmFromPrototype(); // ������Ÿ���� Ŭ������ Ǯ�� ä��
	void PrewarmFromNoPrototype(); // ������Ÿ�� ���� Ǯ�� ä��
	void SettingInOneLine(); // ���� + ������Ÿ�� ����
}

