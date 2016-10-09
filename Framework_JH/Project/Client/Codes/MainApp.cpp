#include "stdafx.h"
#include "MainApp.h"

CMainApp::CMainApp()
{

}

CMainApp::~CMainApp()
{
	Release_MainApp();
}

HRESULT CMainApp::Init_MainApp()
{
	return S_OK;
}

void CMainApp::Update_MainApp()
{

}

void CMainApp::Render_MainApp()
{
}

void CMainApp::Release_MainApp()
{

}

CMainApp* CMainApp::Create(void)
{
	CMainApp*	pMainApp = new CMainApp;

	if (FAILED(pMainApp->Init_MainApp()))
	{
		//MessageBox(NULL, L"MainApp Create Failed!!", L"System Message", MB_OK);
		MSG_BOX(L"MainApp Create Failed!!");
		Safe_Delete(pMainApp);
		//delete pMainApp;
		//pMainApp = NULL;		
	}

	return pMainApp;
}