#include "stdafx.h"
#include "Back.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "MapToolView.h"
#include "MainFrm.h"
#include "StaticObject.h"
#include "ResourcesMgr.h"
#include "Terrain.h"
#include "ObjectTool.h"
#include "MyForm.h"
#include "VIBuffer.h"
#include "StaticMesh.h"

CBack::CBack()
	:m_pTerrainVtx(NULL)
	,m_pTerrain(NULL)
{
}


CBack::~CBack()
{
}

HRESULT CBack::Initialize(void)
{
	

	HRESULT hr = NULL;

	m_pMouseCol = new CMouseCol();
	FAILED_CHECK_RETURN_MSG(m_pMouseCol->Initialize(), E_FAIL, L"MouseCol Init Failed");

	//Texture
	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Terrain"
		, L"../Resource/Terrain/norumac2.png");
	FAILED_CHECK_RETURN(hr, E_FAIL);



	//Buffer
	hr = CResourcesMgr::GetInstance()->AddBuffer(
		RESOURCE_STAGE, 
		BUFFER_TERRAIN, 
		L"Buffer_RcTerrain", 
		VERTEXCOUNTX, 
		VERTEXCOUNTX);
	FAILED_CHECK_RETURN(hr, E_FAIL);
	/*hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Town"
		, "..\\Resource\\"
		, "Town.FBX");
	FAILED_CHECK_RETURN(hr, );*/
	
	if (FAILED(CreateObj()))
		return E_FAIL;



	return S_OK;
}

int CBack::Update(void)
{
	CObjMgr::GetInstance()->Update();

	/*list<CObj*>::iterator iter = m_ToolObjList.begin();
	list<CObj*>::iterator iter_end = m_ToolObjList.end();

	for (iter; iter != iter_end; ++iter)
		(*iter)->Update();*/

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


	CObj* pObj = NULL;

	pObj = CTerrain::Create();
	if (pObj == NULL)
		return E_FAIL;

	m_pTerrain = ((CTerrain*)pObj);

	CObjMgr::GetInstance()->AddObject(L"Terrain", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);



	return S_OK;
}


void CBack::SetMainView(CMapToolView* pMainView)
{
	m_pMainView = pMainView;
}

void CBack::ConstObjectMode()
{
	CObjectTool* pObjectTool = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_Tab1;

	if(TRUE == pObjectTool ->m_ObjModeRadio[1].GetCheck())
	{
		//설치
		CRenderMgr* pRenderer = CRenderMgr::GetInstance();
		D3DXVECTOR3 vPos;

		if (NULL == m_pTerrainVtx)
		{
			m_pTerrainVtx = new VTXTEX[VERTEXCOUNTX * VERTEXCOUNTZ];
			m_pMouseCol->SetSize(VERTEXCOUNTX, VERTEXCOUNTZ, 1);

			m_pTerrainVtx = m_pTerrain->GetVertex();
		}

		m_pMouseCol->PickTerrain(&vPos, m_pTerrainVtx);
		cout << vPos.x << "/" << vPos.y << "/" << vPos.z << endl;
		CObj* pObj = NULL;
		
		const TCHAR* strKey = (TCHAR*)(LPCTSTR)pObjectTool->m_strCurKey;

		if (TRUE == pObjectTool->m_ObjTypeRadio[0].GetCheck())
		{
			//스태틱
			TCHAR* pp = new TCHAR[50];
			wsprintf(pp, pObjectTool->m_strCurKey);
			pObj = CStaticObject::Create(pp, vPos);
			CObjMgr::GetInstance()->AddObject(pp, pObj);
			pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);
			m_ToolObjList.push_back(pObj);
		}
		else
		{
			//다이나믹
		}
	}
	else if (TRUE == pObjectTool->m_ObjModeRadio[2].GetCheck())
	{
		//선택,수정
		D3DXVECTOR3 vIndex;

		list<CObj*>::iterator iter = m_ToolObjList.begin();
		list<CObj*>::iterator iter_end = m_ToolObjList.end();

		const CComponent* pInfoComponent;
		const CComponent* pMeshComponent;

		for (iter; iter != iter_end; ++iter)
		{
			pInfoComponent = (*iter)->GetComponent(L"Transform");
			pMeshComponent = (*iter)->GetComponent(L"Mesh");
			D3DXVECTOR3 temp;

			if (TRUE == pObjectTool->m_ObjTypeRadio[0].GetCheck())
			{
				//스태틱
				if (m_pMouseCol->MeshPick(&temp, (*iter)->GetInfo(), &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
				{
					break;
				}
			}
			else if (TRUE == pObjectTool->m_ObjTypeRadio[1].GetCheck())
			{
				//다이나믹
			}
		}
		if (iter == iter_end)
		{
			pObjectTool->m_stCurrentMeshKey = L"";
			pObjectTool->SetCurObject();
			pObjectTool->OnInfoReset();

			return;
		}


		pObjectTool->m_stCurrentMeshKey = (LPCTSTR)(*iter)->GetMeshKey();
		pObjectTool->SetCurObject(*iter);
		pObjectTool->OnInfoReset();
		
		//const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");


	}

	else if (TRUE == pObjectTool->m_ObjModeRadio[0].GetCheck())
	{
		// 마우스좌표 받기 및 수정
		D3DXVECTOR3 vIndex;

		if (NULL == m_pTerrainVtx)
		{
			m_pTerrainVtx = new VTXTEX[VERTEXCOUNTX * VERTEXCOUNTZ];
			m_pMouseCol->SetSize(VERTEXCOUNTX, VERTEXCOUNTZ, 1);

			m_pTerrainVtx = m_pTerrain->GetVertex();
		}

		m_pMouseCol->PickTerrain(&vIndex, m_pTerrainVtx);

		pObjectTool->SetPickPos(vIndex);
	}

	else if (TRUE == pObjectTool->m_ObjModeRadio[4].GetCheck())
	{
		// 삭제
		D3DXVECTOR3 vIndex;

		list<CObj*>::iterator iter = m_ToolObjList.begin();
		list<CObj*>::iterator iter_end = m_ToolObjList.end();


		const CComponent* pMeshComponent;
		for (iter; iter != iter_end; ++iter)
		{
			pMeshComponent = (*iter)->GetComponent(L"Mesh");

			D3DXVECTOR3 temp;
			if (TRUE == pObjectTool->m_ObjTypeRadio[0].GetCheck())
			{
				//스태틱
				if (m_pMouseCol->MeshPick(&temp, (*iter)->GetInfo(), &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
				{
					break;
				}
			}
		}

		if (iter == iter_end)
		{
			pObjectTool->m_stCurrentMeshKey = L"";
			pObjectTool->SetCurObject();
			pObjectTool->OnInfoReset();
			return;
		}

		pObjectTool->SetCurObject(*iter);
		pObjectTool->OnInfoReset();


		TCHAR tcDelKey[256];
		
		_tcscpy_s(tcDelKey, (*iter)->GetMeshKey());

		CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);

		CObjMgr::GetInstance()->DeleteByCompare(*iter);
		m_ToolObjList.erase(iter);

		//::Safe_Release(*iter);
		

		pObjectTool->m_stCurrentMeshKey = L"";
		pObjectTool->SetCurObject();
		pObjectTool->OnInfoReset();
	}


}
