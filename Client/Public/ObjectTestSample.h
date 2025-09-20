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

// ------ 테스트 함수 -----------

namespace OBT
{
	void SpawnFromType_NOPrototype(); // 타입으로 바로 스폰 -> 프로토타입 없이
	void SpawnFromPrototype(); // 프로토타입에서 스폰
	void PrewarmFromPrototype(); // 프로토타입의 클론으로 풀을 채움
	void PrewarmFromNoPrototype(); // 프로토타입 없이 풀을 채움
	void SettingInOneLine(); // 정의 + 프로토타입 생성
}

