#include "stdafx.h"
#include "Logo.h"
#include "RcTex.h"
#include "Texture.h"
#include "Shader.h"

#include "ShaderMgr.h"
#include "SceneMgr.h"
#include "Device.h"
#include "Loading.h"
#include "ResourcesMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "LogoBack.h"
#include "ObjMgr.h"
#include "AnimationMgr.h"
#include "Stage.h"

CLogo::CLogo()
	: m_pLoading(NULL)
	, m_bDynamicLoading(false)
{
}


CLogo::~CLogo()
{
	Release();
}

HRESULT CLogo::Initialize(void)
{
	HRESULT hr = NULL;


	hr = CResourcesMgr::GetInstance()->AddTexture(RESOURCE_LOGO,
		L"Texture_Logo",
		L"../Resource/Logo.png");

	if (FAILED(hr))
		return E_FAIL;

	m_pLoading = CLoading::Create(CLoading::LOADING_STAGE);

	FAILED_CHECK_RETURN_MSG(Add_Environment_Layer(), E_FAIL, L"Environment_Layer Add false");
	FAILED_CHECK_RETURN_MSG(Add_GameLogic_Layer(), E_FAIL, L"GameLogic_Layer Add false");
	//FAILED_CHECK_RETURN_MSG(Add_Light(), E_FAIL, L"Light Add false");
	
	return S_OK;
}

int CLogo::Update(void)
{
	/*if (GetAsyncKeyState(VK_RETURN))
		CSceneMgr::GetInstance()->ChangeScene(SCENE_STAGE);*/

	if (m_pLoading->GetComplete() == true)
	{
		// 일단 여기다 넣고 실행해보고, 결과 함 보자 ㄱ


		if (m_bDynamicLoading == false)
			Add_Dynamic_Buffer();

		if (m_bDynamicLoading == true)
		{
			if (GetAsyncKeyState(VK_RETURN))
			{
				CSceneMgr::GetInstance()->ChangeScene(SCENE_STAGE);
			}
		}
		return 0;

		//스테이지마다 하는건 구조좀 바꿔야해서 일단 가라쳐서 로드.
	}

	return 0;
}

void CLogo::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CLogo::Release(void)
{
	::Safe_Delete(m_pLoading);

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->end();

	for (iter; iter != iter_end; ++iter)
	{
		CRenderMgr::GetInstance()->DelRenderGroup(TYPE_UI, *iter);
		::Safe_Delete(*iter);
	}

	CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->clear();

}

CLogo * CLogo::Create(void)
{
	CLogo* pLogo = new CLogo;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);

	}
	return pLogo;
}

HRESULT CLogo::Add_Environment_Layer(void)
{
	return S_OK;
}

HRESULT CLogo::Add_GameLogic_Layer(void)
{
	/*CLayer*		pLayer = Engine::CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);*/

	CObj*	pObj = NULL;

	pObj = CLogoBack::Create();
	NULL_CHECK_RETURN_MSG(pObj, E_FAIL, L"CLogoBack 생성 실패");
	CObjMgr::GetInstance()->AddObject(L"LogoBack", pObj);

	return S_OK;
}

HRESULT CLogo::Add_Dynamic_Buffer(void)
{
	if (m_bDynamicLoading == false)
	{
		HRESULT hr = NULL;

		cout << "DynamicBufferLoading" << endl;
		vector<string> vecAni;

		//여기에 아마 메모장통해서 벡터에다가 꼴아박아야할거같음. 일단 가라침

		vecAni.push_back("player_normalidle");
		vecAni.push_back("player_fightrun");
		vecAni.push_back("player_normalattack1");

		hr = CResourcesMgr::GetInstance()->AddMesh(
			RESOURCE_STAGE,
			MESH_DYNAMIC,
			L"Player_IDLE",
			"../Resource/Mesh/",
			"", vecAni);
		FAILED_CHECK_RETURN(hr, E_FAIL);


		CAnimationMgr::GetInstance()->AddAnimation(L"Player", &vecAni);

		vecAni.clear();

		cout << "SceneLoading" << endl;

		CScene* pScene = NULL;

		pScene = CStage::Create();
		CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);

		//lstrcpy(m_szLoadMessage, L"로딩 완료...");
		cout << "Loading End" << endl;


		m_bDynamicLoading = true;
	}

	return S_OK;
}