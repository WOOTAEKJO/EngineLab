#include "..\Public\OB_Test_Monster.h"

HRESULT COB_Test_Monster::InitPrototype(OTMonsterProto p)
{
	m_proto = p;

	return S_OK;
}

HRESULT COB_Test_Monster::InitInstance(OTMonsterSpawn s)
{
	m_spawn = s;

	return S_OK;
}

void COB_Test_Monster::ResetInstance()
{
}
