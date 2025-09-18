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
	CGameInstance::Get_Instance()->Tick(dt);

	//CGameInstance::Get_Instance()->Service().Objects().
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

