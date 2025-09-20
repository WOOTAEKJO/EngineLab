#pragma once
#include "Client_Defines.h"
#include "IGameObject.h"

struct OTMonsterSpawn { float x, y; int kind; };
struct OTMonsterProto { string prefab, animSet; };

class COB_Test_Monster : public IGameObject
{
protected:
	COB_Test_Monster() = default;
	COB_Test_Monster(const COB_Test_Monster&) = default;
	~COB_Test_Monster() = default;

public:
	virtual HRESULT InitPrototype(OTMonsterProto p);
	virtual HRESULT InitInstance(OTMonsterSpawn s);
	virtual void ResetInstance();

private:
	OTMonsterProto m_proto;
	OTMonsterSpawn m_spawn;
};

