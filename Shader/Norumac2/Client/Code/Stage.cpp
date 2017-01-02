#include "stdafx.h"
#include "Stage.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "Terrain.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "Flower.h"
#include "Player.h"

CStage::CStage()
{
}


CStage::~CStage()
{
}

HRESULT CStage::Initialize(void)
{
	if (FAILED(CreateObj()))
		return E_FAIL;

	return S_OK;
}

int CStage::Update(void)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		CSceneMgr::GetInstance()->ChangeScene(SCENE_LOGO);

	CObjMgr::GetInstance()->Update();

	return 0;
}

void CStage::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CStage::Release(void)
{
}

CStage * CStage::Create(void)
{

	CStage* pLogo = new CStage;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);
	}
	return pLogo;
	
}

HRESULT CStage::CreateObj(void)
{
	CRenderMgr* pRenderer = CRenderMgr::GetInstance();
	//ÅÍ·¹ÀÎ
	CObj* pObj = NULL;
	pObj = CTerrain::Create();
	if (pObj == NULL)
		return E_FAIL;

	CObjMgr::GetInstance()->AddObject(L"Terrain", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);

	for (int i = 0; i < 20; ++i)
	{
		pObj = CFlower::Create();
		if (pObj == NULL)
			return E_FAIL;

		float		fX = float(rand() % VERTEXCOUNTX);
		float		fZ = float(rand() % VERTEXCOUNTZ);

		pObj->SetPos(D3DXVECTOR3(fX, 0.f, fZ));

		CObjMgr::GetInstance()->AddObject(L"Flower", pObj);
		pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);
	}

	pObj = CPlayer::Create();
	CObjMgr::GetInstance()->AddObject(L"Player", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);
	


	return S_OK;
}
