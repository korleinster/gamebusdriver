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
#include "Info.h"
#include "ResourcesMgr.h"
#include "Frustum.h"
#include "FaceUI.h"
#include "LightMgr.h"
#include "BaseUI.h"
#include "HpBar.h"
#include "HpZero.h"
#include "FeverBar.h"

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

	

	return S_OK;
}

int CStage::Update(void)
{
	if (m_bFirstLogin == false)
	{
		
		m_bFirstLogin = true;
	}

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
	//CResourcesMgr::GetInstance()->ResourceReset(RESOURCE_STATIC);
	//CResourcesMgr::GetInstance()->ResourceReset(RESOURCE_STAGE);
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
	//

	pObj = CHpBar::Create();
	if (pObj == NULL)
		return E_FAIL;
	CObjMgr::GetInstance()->AddObject(L"HpBar", pObj);

	pObj = CHpZero::Create();
	if (pObj == NULL)
		return E_FAIL;
	CObjMgr::GetInstance()->AddObject(L"HpZero", pObj);

	pObj = CFeverBar::Create();
	if (pObj == NULL)
		return E_FAIL;
	CObjMgr::GetInstance()->AddObject(L"FeverBar", pObj);

	pObj = CFaceUI::Create();
	if (pObj == NULL)
		return E_FAIL;
	CObjMgr::GetInstance()->AddObject(L"FaceUI", pObj);

	pObj = CBaseUI::Create();
	if (pObj == NULL)
		return E_FAIL;
	CObjMgr::GetInstance()->AddObject(L"BaseUI", pObj);

	

	pObj = CPlayer::Create();
	pObj->SetPos(D3DXVECTOR3(155.f, 0.f, 400.f));
	CObjMgr::GetInstance()->AddObject(L"Player", pObj);

	CCamera::GetInstance()->SetCameraTarget(pObj->GetInfo());


	DataLoad();
	


	return S_OK;
}

void CStage::DataLoad(void)
{
	HANDLE	hFile = CreateFile(L"..\\Resource\\Data\\Norumac2.dat", GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CLightMgr* pLightMgr = CLightMgr::GetInstance();
	DWORD dwByte;

	int iObjSize = 0;
	ReadFile(hFile, &iObjSize, sizeof(int), &dwByte, NULL);

	for (int i = 0; i < iObjSize; ++i)
	{
		TCHAR* pObjectKey = new TCHAR[50];
		ReadFile(hFile, pObjectKey, sizeof(TCHAR) * 50, &dwByte, NULL);

		int iNum;
		ReadFile(hFile, &iNum, sizeof(int), &dwByte, NULL);
		if(0==iNum)
			continue;
		CObj* pGameObject = NULL;

		for (int j = 0; j < iNum; ++j)
		{
			pGameObject = CStaticObject::Create(pObjectKey);
			CObjMgr::GetInstance()->AddObject(pObjectKey, pGameObject);
			CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pGameObject);

			const CComponent* pComponent = pGameObject->GetComponent(L"Transform");
			
			ReadFile(hFile, ((CInfo*)pComponent)->m_fAngle, sizeof(float) * ANGLE_END, &dwByte, NULL);
			ReadFile(hFile, ((CInfo*)pComponent)->m_vScale, sizeof(D3DXVECTOR3), &dwByte, NULL);
			ReadFile(hFile, ((CInfo*)pComponent)->m_vPos, sizeof(D3DXVECTOR3), &dwByte, NULL);
			ReadFile(hFile, ((CInfo*)pComponent)->m_vDir, sizeof(D3DXVECTOR3), &dwByte, NULL);
			ReadFile(hFile, ((CInfo*)pComponent)->m_matWorld, sizeof(D3DXMATRIX), &dwByte, NULL);

			

			if (0 == wcscmp(pObjectKey, L"streetlamp"))
			{
				D3DXVECTOR3 vPos;
				vPos.x = ((CInfo*)pComponent)->m_vPos.x - 0.5f;
				vPos.y = ((CInfo*)pComponent)->m_vPos.y+1;
				vPos.z = ((CInfo*)pComponent)->m_vPos.z;
				pLightMgr->AddPointLight(vPos, 3.f, D3DXVECTOR3(1.0f, 0.0f, 0.0f));
			}
		}
	}
	CloseHandle(hFile);
}

