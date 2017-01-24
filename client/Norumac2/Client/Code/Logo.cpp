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

CLogo::CLogo()
	: m_pLoading(NULL)
{
}


CLogo::~CLogo()
{
}

HRESULT CLogo::Initialize(void)
{
	HRESULT hr = NULL;


	hr = CResourcesMgr::GetInstance()->AddTexture(RESOURCE_LOGO,
		L"Texture_Logo",
		L"../Resource/Logo.jpg");

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

	if (GetAsyncKeyState(VK_RETURN))
	{
		if (m_pLoading->GetComplete() == true)
		{
			CSceneMgr::GetInstance()->ChangeScene(SCENE_STAGE);
			return 0;
		}
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
	::Safe_Release(m_pLoading);

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->end();

	for (iter; iter != iter_end;)
	{
		CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
		::Safe_Release(*iter);
		CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->erase(iter++);

	}

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