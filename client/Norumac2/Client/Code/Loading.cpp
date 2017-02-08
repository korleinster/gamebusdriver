#include "stdafx.h"
#include "Loading.h"

#include "Include.h"
#include <process.h>
#include "ResourcesMgr.h"
#include "Scene.h"
#include "Stage.h"
#include "SceneMgr.h"
#include "AnimationMgr.h"

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

	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Player"
		, L"../Resource/MeshImage/newplayer.png");


	//lstrcpy(m_szLoadMessage, L"버퍼 로딩중...");
	cout << "StaticBufferLoading" << endl;
	//Buffer
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Town"
		, "../Resource/Mesh/"
		, "Town01.FBX");
	FAILED_CHECK_RETURN(hr, );









	//다이나믹은 ㅅㅂ 스레드 ㅈ같아서 스레드로딩안함. 


	cout << "Thread Loading End" << endl;
	m_bComplete = true;


	if (m_serverConnected == false) {
		g_client.Init(g_hWnd);
		m_serverConnected = true;
	}
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




//#include "stdafx.h"
//#include "Loading.h"
//
//#include "Include.h"
//#include <process.h>
//#include "ResourcesMgr.h"
//#include "Scene.h"
//#include "Stage.h"
//#include "SceneMgr.h"
//#include "AnimationMgr.h"
//
//CLoading::CLoading(/*LOADINGID eLoadID*/)
//	: /*m_eLoadID(NULL)*/
//	//, m_hThread(NULL)
//	m_bComplete(false)
//{
//	ZeroMemory(m_szLoadMessage, sizeof(TCHAR) * 128);
//	//ZeroMemory(&m_CSKey, sizeof(CRITICAL_SECTION));
//}
//
//CLoading::~CLoading(void)
//{
//
//}
//
//bool CLoading::GetComplete(void)
//{
//	return m_bComplete;
//}
//
//const TCHAR* CLoading::GetLoadMessage(void)
//{
//	return m_szLoadMessage;
//}
//
//HRESULT CLoading::InitLoading(LOADINGID eLoadID)
//{
//	//InitializeCriticalSection(&m_CSKey);
//
//	//m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingFunction, this, 0, NULL);
//
//	HRESULT hr = LoadingFunction(eLoadID);
//	if (hr != S_OK)
//		return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CLoading::StageLoading()
//{
//	HRESULT hr = NULL;
//	//lstrcpy(m_szLoadMessage, L"텍스쳐 로딩중...");
//	cout << "TextureLoading" << endl;
//	//Texture
//	hr = CResourcesMgr::GetInstance()->AddTexture(
//		RESOURCE_STAGE
//		, L"Texture_Town"
//		, L"../Resource/MeshImage/Town.png");
//	FAILED_CHECK_RETURN(hr, E_FAIL);
//
//	hr = CResourcesMgr::GetInstance()->AddTexture(
//		RESOURCE_STAGE
//		, L"Texture_Flower"
//		, L"../Resource/Flower.png");
//	FAILED_CHECK_RETURN(hr, E_FAIL);
//
//	hr = CResourcesMgr::GetInstance()->AddTexture(
//		RESOURCE_STAGE
//		, L"Texture_Player"
//		, L"../Resource/MeshImage/Player.png");
//
//
//	//lstrcpy(m_szLoadMessage, L"버퍼 로딩중...");
//	cout << "StaticBufferLoading" << endl;
//	//Buffer
//	hr = CResourcesMgr::GetInstance()->AddMesh(
//		RESOURCE_STAGE,
//		MESH_STATIC
//		, L"Mesh_Town"
//		, "../Resource/Mesh/"
//		, "Town.FBX");
//	FAILED_CHECK_RETURN(hr, E_FAIL);
//
//
//	cout << "DynamicBufferLoading" << endl;
//	vector<string> vecAni;
//
//	//여기에 아마 메모장통해서 벡터에다가 꼴아박아야할거같음. 일단 가라침
//
//	vecAni.push_back("Player");
//
//	hr = CResourcesMgr::GetInstance()->AddMesh(
//		RESOURCE_STAGE,
//		MESH_DYNAMIC,
//		L"Player_IDLE",
//		"../Resource/Mesh/",
//		"", vecAni);
//	FAILED_CHECK_RETURN(hr, E_FAIL);
//
//
//	CAnimationMgr::GetInstance()->AddAnimation(L"Player", &vecAni);
//
//	vecAni.clear();
//
//	cout << "SceneLoading" << endl;
//
//	CScene* pScene = NULL;
//
//	pScene = CStage::Create();
//	CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);
//
//	//lstrcpy(m_szLoadMessage, L"로딩 완료...");
//	cout << "Loading End" << endl;
//
//	return S_OK;
//}
//
//CLoading* CLoading::Create(LOADINGID eLoadID)
//{
//	CLoading*	pLoading = new CLoading();
//	if (FAILED(pLoading->InitLoading(eLoadID)))
//		::Safe_Release(pLoading);
//
//	return pLoading;
//}
//
//HRESULT /*WINAPI*/ CLoading::LoadingFunction(/*void* pArg*/LOADINGID eLoadID)
//{
//	//CLoading*		pLoading = (CLoading*)pArg;
//
//	HRESULT hr = NULL;
//
//	switch (eLoadID)
//	{
//	case CLoading::LOADING_STAGE:
//		hr = StageLoading();
//		break;
//
//	case CLoading::LOADING_STAGE1:
//		break;
//	}
//
//	if (hr != S_OK)
//		return E_FAIL;
//	return S_OK;
//}
//
//void CLoading::Release(void)
//{
//	//DeleteCriticalSection(&m_CSKey);
//	//WaitForSingleObject(m_hThread, INFINITE);
//	//CloseHandle(m_hThread);
//}
