#include "stdafx.h"
#include "MainApp.h"

#include "DeviceMgr.h"

CMainApp::CMainApp()
{

}

CMainApp::~CMainApp()
{
	Release_MainApp();
}

HRESULT CMainApp::Init_MainApp()
{
	// =====For.Device Init
	CDeviceMgr::GetInstance()->Init(CDeviceMgr::WIN_MODE_WIN);
	m_pDevice = CDeviceMgr::GetInstance();

	return S_OK;
}

void CMainApp::Update_MainApp()
{

}

void CMainApp::Render_MainApp()
{
	// 이부분 RenderMgr로 옮길 예정
	m_pDevice->Render_Begin();

	m_pDevice->Render_End();
}

void CMainApp::Release_MainApp()
{
	CDeviceMgr::DestroyInstance();
}

CMainApp* CMainApp::Create(void)
{
	CMainApp*	pMainApp = new CMainApp;

	if (FAILED(pMainApp->Init_MainApp()))
	{
		MSG_BOX(L"MainApp Create Failed!!");
		Safe_Delete(pMainApp);
	}

	return pMainApp;
}