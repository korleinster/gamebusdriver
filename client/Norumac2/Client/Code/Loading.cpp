#include "stdafx.h"
#include "Loading.h"

#include "Include.h"
#include <process.h>
#include "ResourcesMgr.h"
#include "Scene.h"
#include "Stage.h"
#include "SceneMgr.h"

CLoading::CLoading(LOADINGID eLoadID)
	: m_eLoadID(eLoadID)
	, m_hThread(NULL)
	, m_bComplete(false)
{
	ZeroMemory(m_szLoadMessage, sizeof(TCHAR) * 128);
	ZeroMemory(&m_CSKey, sizeof(CRITICAL_SECTION));
}

CLoading::~CLoading(void)
{

}

bool CLoading::GetComplete(void)
{
	return m_bComplete;
}

const TCHAR* CLoading::GetLoadMessage(void)
{
	return m_szLoadMessage;
}

HRESULT CLoading::InitLoading(void)
{
	InitializeCriticalSection(&m_CSKey);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingFunction, this, 0, NULL);
	if (m_hThread == NULL)
		return E_FAIL;

	return S_OK;
}

void CLoading::StageLoading(void)
{
	HRESULT hr = NULL;

	//lstrcpy(m_szLoadMessage, L"텍스쳐 로딩중...");
	cout << "TextureLoading" << endl;
	//Texture
	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Town"
		, L"../Resource/MeshImage/Town.png");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Flower"
		, L"../Resource/Flower.png");
	FAILED_CHECK_RETURN(hr, );


	//lstrcpy(m_szLoadMessage, L"버퍼 로딩중...");
	cout << "BufferLoading" << endl;
	//Buffer
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Town"
		, "../Resource/Mesh/"
		, "Town.FBX");
	FAILED_CHECK_RETURN(hr, );
	
	
	cout << "SceneLoading" << endl;

	CScene* pScene = NULL;
	
	pScene = CStage::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);

	//lstrcpy(m_szLoadMessage, L"로딩 완료...");
	cout << "Loading End" << endl;
	m_bComplete = true;
}

CLoading* CLoading::Create(LOADINGID eLoadID)
{
	CLoading*	pLoading = new CLoading(eLoadID);
	if (FAILED(pLoading->InitLoading()))
		::Safe_Release(pLoading);

	return pLoading;
}

UINT WINAPI CLoading::LoadingFunction(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	switch (pLoading->m_eLoadID)
	{
	case CLoading::LOADING_STAGE:
		pLoading->StageLoading();
		break;

	case CLoading::LOADING_STAGE1:
		break;
	}
	return 0;
}

void CLoading::Release(void)
{
	DeleteCriticalSection(&m_CSKey);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
}
