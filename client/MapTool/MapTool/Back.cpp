#include "stdafx.h"
#include "Back.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "MapToolView.h"
#include "MainFrm.h"
#include "StaticObject.h"

CBack::CBack()
{
}


CBack::~CBack()
{
}

HRESULT CBack::Initialize(void)
{
	if (FAILED(CreateObj()))
		return E_FAIL;



	return S_OK;
}

int CBack::Update(void)
{
	CObjMgr::GetInstance()->Update();


	return 0;
}

void CBack::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CBack::Release(void)
{
}

CBack * CBack::Create(void)
{

	CBack* pLogo = new CBack;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);
	}
	return pLogo;

}

HRESULT CBack::CreateObj(void)
{
	CRenderMgr* pRenderer = CRenderMgr::GetInstance();
	//ÅÍ·¹ÀÎ
	CObj* pObj = NULL;
	pObj = CStaticObject::Create();
	if (pObj == NULL)
		return E_FAIL;

	CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);



	return S_OK;
}


void CBack::SetMainView(CMapToolView* pMainView)
{
	m_pMainView = pMainView;
}
