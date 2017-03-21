#include "stdafx.h"
#include "Stage.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "Terrain.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "Flower.h"
#include "Player.h"
#include "StaticObject.h"
#include "Camera.h"

CStage::CStage()
	: m_bFirstLogin(false)
{
}


CStage::~CStage()
{
}

HRESULT CStage::Initialize(void)
{
	if (FAILED(CreateObj()))
		return E_FAIL;

	// 서버와 통신을 위해서, 윈도우 핸들 값을 받아온다.
	

	return S_OK;
}

int CStage::Update(void)
{
	if (m_bFirstLogin == false)
	{
		
		m_bFirstLogin = true;
	}

	CObjMgr::GetInstance()->Update();


	if (GetAsyncKeyState('0'))
	{
		list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"Flower")->begin();
		list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"Flower")->end();

		for (iter; iter != iter_end;)
		{
			CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
			::Safe_Release(*iter);
			CObjMgr::GetInstance()->Get_ObjList(L"Flower")->erase(iter++);
			cout << CObjMgr::GetInstance()->Get_ObjList(L"Flower")->size() << endl;
		}		
		
	}

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
	//터레인
	CObj* pObj = NULL;

	///////////////야매///////////////
	TCHAR  szMeshKey[MAX_PATH] = L"";
	TCHAR  szTexKey[MAX_PATH] = L"";
	int iHigh = 0;

	pObj = CTerrain::Create();
	if (pObj == NULL)
		return E_FAIL;

	CObjMgr::GetInstance()->AddObject(L"Terrain", pObj);

	
	

	/*for (int i = 0; i < 20; ++i)
	{
		pObj = CFlower::Create();
		if (pObj == NULL)
			return E_FAIL;

		float		fX = float(rand() % VERTEXCOUNTX);
		float		fZ = float(rand() % VERTEXCOUNTZ);

		pObj->SetPos(D3DXVECTOR3(fX, 0.f, fZ));

		CObjMgr::GetInstance()->AddObject(L"Flower", pObj);
	}*/

	pObj = CPlayer::Create();
	pObj->SetPos(D3DXVECTOR3(10.f, 0.f, 10.f));
	CObjMgr::GetInstance()->AddObject(L"Player", pObj);

	CCamera::GetInstance()->SetCameraTarget(pObj->GetInfo());
	


	return S_OK;
}

