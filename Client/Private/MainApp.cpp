#include "..\Public\MainApp.h"
#include "GameInstance.h"

#include "ManagerContainer.h"
#include "TimerManager.h"
#include "EngineAPI.h"

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

	int a = 0;
}

void CMainApp::Render()
{

}

