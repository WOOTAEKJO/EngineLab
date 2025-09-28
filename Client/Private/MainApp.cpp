#include "..\Public\MainApp.h"
#include "GameInstance.h"

#include "ManagerContainer.h"
#include "TimerManager.h"
#include "EngineAPI.h"

#include "../Public/ObjectTestSample.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Init()
{
	CGameInstance::Get_Instance()->Init();
	
	return S_OK;
}

void CMainApp::Tick(_double dt)
{
	// Log::FrameStart(f);
	// Log::Write(INFO,TIME,"fixed_step"{step=1.dt=16.667})

	CGameInstance::Get_Instance()->Tick(dt);

	// Log::FrameEnd(f, cpuMs)
	// Tel::FlushFrame()
}

void CMainApp::Render()
{

}

void CMainApp::Test()
{
	using namespace OBT;

	OBT::SpawnFromType_NOPrototype();
	OBT::SpawnFromPrototype();
	OBT::PrewarmFromPrototype();
	OBT::PrewarmFromNoPrototype();
}

void CMainApp::Test2()
{
	using namespace OBT;

	auto& objs = CGameInstance::Get_Instance()->Service().Objects();

	OBT::LoadStage(objs);
	OBT::SpawnEnemy(objs);
	OBT::CloneEnemy(objs);
	OBT::Fire(objs);
}

