#include "stdafx.h"
#include "MainApp.h"

CMainApp::CMainApp()
{

}

CMainApp::~CMainApp()
{
	Release();
}

HRESULT CMainApp::InitApp()
{
	return S_OK;
}

void CMainApp::Update()
{

}

void CMainApp::Render()
{
}

void CMainApp::Release()
{

}

CMainApp* CMainApp::Create(void)
{
	CMainApp*	pMainApp = new CMainApp;

	if (FAILED(pMainApp->InitApp()))
	{
		MessageBox(NULL, L"MainApp Create Failed", L"System Message", MB_OK);
		delete pMainApp;
		pMainApp = NULL;		
	}

	return pMainApp;
}