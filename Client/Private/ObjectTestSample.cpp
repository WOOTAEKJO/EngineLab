#include "../Public/ObjectTestSample.h"
#include "GameInstance.h"
#include "EngineAPI.h"

HRESULT OB_Test_Bullet::InitPrototype(OTBulletProtu p)
{
	m_proto = p;

	return S_OK;
}

HRESULT OB_Test_Bullet::InitInstance(OTBulletSpawn s)
{
	m_spawn = s;

	return S_OK;
}

void OB_Test_Bullet::ResetInstance()
{
}

void OBT::SpawnFromType_NOPrototype()
{
	auto& objs = CGameInstance::Get_Instance()->Service().Objects();

	objs.DefineSpawn<OB_Test_Bullet, OTBulletSpawn>();
	ObjectMeta meta{};
	meta.layerID = 2;

	OB_Test_Bullet* b = objs.Spawn<OB_Test_Bullet>(meta, OTBulletSpawn{ 1,1,1,0,0,1,5 });

	objs.Release(b, "bullet.basic");
}

void OBT::SpawnFromPrototype()
{
	auto& objs = CGameInstance::Get_Instance()->Service().Objects();

	HRESULT ok = S_OK;

	ok = objs.DefineSpawn<OB_Test_Bullet, OTBulletSpawn>(); // 타입 등록
	if (FAILED(ok))	 return;

	ok = objs.DefinePrototypeInit<OB_Test_Bullet, OTBulletProtu>(); // 프로토타입 초기화 등록
	if (FAILED(ok))	 return;

	ok = objs.CreatePrototype<OB_Test_Bullet>("bullet.fast", OTBulletProtu{ "mesh/bullet.fbx","mat/bullet.mat" });
	if (FAILED(ok))	 return;

	OB_Test_Bullet* b = static_cast<OB_Test_Bullet*>(objs.Clone("bullet.fast", ObjectMeta{ .layerID = 2 },
		OTBulletSpawn{ 0,0,0, 1,0,0, 1 }));
}

void OBT::PrewarmFromPrototype()
{
	auto& objs = CGameInstance::Get_Instance()->Service().Objects();

	objs.PrimePoolProto("bullet.fast", 200);

	OB_Test_Bullet* b = static_cast<OB_Test_Bullet*>(objs.Acquire("bullet.fast",
		OTBulletSpawn{ 10,0,0, 0,1,0, 2 }));

	objs.Release(b, "bullet.fast");
}

void OBT::PrewarmFromNoPrototype()
{
	auto& objs = CGameInstance::Get_Instance()->Service().Objects();

	objs.DefineSpawn<COB_Test_Monster, OTMonsterSpawn>();
	objs.PrimePool<COB_Test_Monster>("monster.basic", 50);

	COB_Test_Monster* m = static_cast<COB_Test_Monster*>(objs.Acquire("monster.basic",
		OTMonsterSpawn{ 5,3,2 }));

	objs.Release(m, "monster.basic");
}

void OBT::SpawnFromType_NOPrototype_API(IObjectService& svc)
{
	API::DefineSpawn<COB_Test_Monster>(svc);
	auto* e = API::Spawn<COB_Test_Monster>(svc);
}

void OBT::SpawnFromPrototype_API(IObjectService& svc)
{
	//OTMonsterSpawn spawn = { 1,1,2 };
	OTMonsterProto proto = { "a","b" };

	API::DefineSpawn<COB_Test_Monster>(svc, OTMonsterSpawn{});
	API::CreateProto<COB_Test_Monster>(svc,"monster.basic",proto);

	auto meta = API::Meta{}.Layer(2).Visible(true).Paused(false).TimeScale(1.f);

	auto* m = API::Clone(svc, "monster.basic", meta, OTMonsterSpawn{1,1,2});

}

void OBT::PrewarmFromPrototype_API(IObjectService& svc)
{
	//API::RegisterType<OB_Test_Bullet>(svc,"bullet.basic",200,)
}

void OBT::Layer_PauseANDVisible_Toggle(IObjectService& svc)
{

}