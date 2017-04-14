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
#include "NaviMgr.h"
#include "StaticMesh.h"
#include "NaviTool.h"
#include "NaviCell.h"

CBack::CBack()
	:m_pTerrainVtx(NULL)
	,m_pTerrain(NULL)
	,m_bPickFirst(false)
	,m_bPickSecond(false)
	,m_bGetMousePt(false)
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
	CNaviMgr::GetInstance()->Update();

	CInterface* pInterFace = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_InterFace;

	MEMORYSTATUS memoryStatus;
	GlobalMemoryStatus(&memoryStatus);

	SIZE_T dwAvailVirtual = memoryStatus.dwAvailVirtual >> 20;
	SIZE_T dwTotalVirtual = memoryStatus.dwTotalVirtual >> 20;

	pInterFace->HardwareUpdate(memoryStatus.dwMemoryLoad, dwTotalVirtual - dwAvailVirtual, dwTotalVirtual*2);
	pInterFace->NaviUpdate(m_bPickFirst, m_bPickSecond);


	return 0;
}

void CBack::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
	CNaviMgr::GetInstance()->Render();
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
				else
					(*iter)->m_fSelect = 0.f;
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
		(*iter)->m_fSelect = 1.f;
		
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

void CBack::NaviMeshMode()
{
	CNaviTool*	pNavimeshTool = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_Tab2;

	D3DXVECTOR3 vIndex;

	if (NULL == m_pTerrainVtx)
	{
		m_pTerrainVtx = new VTXTEX[VERTEXCOUNTX * VERTEXCOUNTZ];
		m_pMouseCol->SetSize(VERTEXCOUNTX, VERTEXCOUNTZ, 1);

		m_pTerrainVtx = m_pTerrain->GetVertex();
	}
	if (TRUE == pNavimeshTool->m_PickingMode[0].GetCheck())
	{
		m_pMouseCol->PickTerrain(&vIndex, m_pTerrainVtx);
	}
	else if (TRUE == pNavimeshTool->m_PickingMode[1].GetCheck())
	{

		list<CObj*>::iterator iter = m_ToolObjList.begin();
		list<CObj*>::iterator iter_end = m_ToolObjList.end();

		const CComponent* pInfoComponent;
		const CComponent* pMeshComponent;

		for (iter; iter != iter_end; ++iter)
		{
			pInfoComponent = (*iter)->GetComponent(L"Transform");
			pMeshComponent = (*iter)->GetComponent(L"Mesh");
			if (m_pMouseCol->MeshPick(&vIndex, (*iter)->GetInfo(), &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
			{
				break;
			}
		}
		if (iter == iter_end)
		{
			m_pMouseCol->PickTerrain(&vIndex, m_pTerrainVtx);
		}
	}

	pNavimeshTool->SetPickPos(vIndex);

	if (vIndex.x < 0.f || vIndex.z < 0.f)
		return;

	if (TRUE == pNavimeshTool->m_NaviMode[1].GetCheck())
	{
		//3점피킹
		if (TRUE == pNavimeshTool->m_CreateMode[0].GetCheck())
		{
			if (!m_bPickFirst)
			{
				CNaviMgr::GetInstance()->GetNearPoint(vIndex, pNavimeshTool->m_fNearRange);
				m_vPoint[0] = vIndex;
				m_bPickFirst = true;
			}
			else if (m_bPickFirst && !m_bPickSecond)
			{
				CNaviMgr::GetInstance()->GetNearPoint(vIndex, pNavimeshTool->m_fNearRange);
				m_vPoint[1] = vIndex;
				m_bPickSecond = true;
			}
			else if (m_bPickFirst && m_bPickSecond)
			{
				CNaviMgr::GetInstance()->GetNearPoint(vIndex, pNavimeshTool->m_fNearRange);
				m_vPoint[2] = vIndex;
				m_bPickFirst = false;
				m_bPickSecond = false;
			}
			BuildNavimesh();
		}
		// 1점 피킹
		if (TRUE == pNavimeshTool->m_CreateMode[1].GetCheck())
		{
			if (CNaviMgr::GetInstance()->GetSize() == 0)
				return;

			m_vPoint[0] = vIndex;

			if (CNaviMgr::GetInstance()->SetOnePoint(m_vPoint[1], m_vPoint[2], m_vPoint[0]))
				BuildNavimesh();
			else
				return;
		}
	}
	else if (TRUE == pNavimeshTool->m_NaviMode[0].GetCheck())
	{
		vector<CNaviCell*>* vecCell = CNaviMgr::GetInstance()->GetCell();
		vector<CNaviCell*>::iterator Navi_iter = vecCell->begin();
		vector<CNaviCell*>::iterator Navi_iter_end = vecCell->end();
		D3DXVECTOR3 temp;
		for (Navi_iter; Navi_iter != Navi_iter_end; ++Navi_iter)
		{
			if (m_pMouseCol->PickNaviCell(&temp, *Navi_iter, &m_iNaviIndex))
			{
				m_pSelectedCell = (*Navi_iter);
				pNavimeshTool->SetPickCell(m_pSelectedCell,m_iNaviIndex);
				break;
			}
			else
				pNavimeshTool->SetPickCell(NULL, -1);
		}
	}
	else if (TRUE == pNavimeshTool->m_NaviMode[3].GetCheck())
	{
		if (TRUE == pNavimeshTool->m_DeleteMode[0].GetCheck())
		{
			// Vtx 삭제
			if (CNaviMgr::GetInstance()->GetSize() == 0)
				return;

			if (!m_bGetMousePt)
			{
				CNaviMgr::GetInstance()->GetNearPoint(vIndex, pNavimeshTool->m_fNearRange);
				m_vGetMousePt = vIndex;
				m_bGetMousePt = true;
			}
			else
			{
				CNaviMgr::GetInstance()->Unlink_Cell();

				CNaviMgr::GetInstance()->DeletePoint(vIndex, m_vGetMousePt);
				m_vGetMousePt = D3DXVECTOR3(0.f, 0.f, 0.f);
				m_bGetMousePt = false;

				CNaviMgr::GetInstance()->ResetIndex();

				CNaviMgr::GetInstance()->Link_Cell();
			}
		}
		if (TRUE == pNavimeshTool->m_DeleteMode[1].GetCheck())
		{
			// Tri 삭제
			if (CNaviMgr::GetInstance()->GetSize() == 0)
				return;



			vector<CNaviCell*>* vecCell = CNaviMgr::GetInstance()->GetCell();
			vector<CNaviCell*>::iterator Navi_iter = vecCell->begin();
			vector<CNaviCell*>::iterator Navi_iter_end = vecCell->end();
			D3DXVECTOR3 temp;
			for (Navi_iter; Navi_iter != Navi_iter_end; ++Navi_iter)
			{
				if (m_pMouseCol->PickNaviCell(&temp, *Navi_iter, &m_iNaviIndex))
				{
					CNaviMgr::GetInstance()->Unlink_Cell();
					Safe_Release(*Navi_iter);
					CNaviMgr::GetInstance()->GetCell()->erase(Navi_iter);
					CNaviMgr::GetInstance()->ResetIndex();
					CNaviMgr::GetInstance()->Link_Cell();
					break;
				}
				else
					return;
			}

			

			//if (!m_bGetMousePt)
			//{
			//	//m_pNaviMgr->GetNearPoint(vIndex);
			//	m_vGetMousePt = vIndex;
			//	m_bGetMousePt = true;
			//}
			//else
			//{
			//	CNaviMgr::GetInstance()->Unlink_Cell();

			//	CNaviMgr::GetInstance()->DeleteTriangle(vIndex, m_vGetMousePt);
			//	m_vGetMousePt = D3DXVECTOR3(0.f, 0.f, 0.f);
			//	m_bGetMousePt = false;

			//	CNaviMgr::GetInstance()->ResetIndex();

			//	CNaviMgr::GetInstance()->Link_Cell();
			//}
		}
	}
	pNavimeshTool->m_iNaviCnt = CNaviMgr::GetInstance()->GetSize();

	((CMainFrame*)AfxGetMainWnd())->m_pMainView->Invalidate(FALSE);
	pNavimeshTool->UpdateData(FALSE);

}

void CBack::BuildNavimesh()
{
	CNaviTool* pNavimeshTool = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_Tab2;

	if (!m_bPickFirst && !m_bPickSecond)
	{
		ComparePoint(m_vPoint[0], m_vPoint[1], m_vPoint[2]);
		CNaviMgr::GetInstance()->Unlink_Cell();
		if (pNavimeshTool->m_CellType[0].GetCheck() == TRUE)
			CNaviMgr::GetInstance()->Add_Cell(&m_vPoint[0], &m_vPoint[1], &m_vPoint[2], TYPE_TERRAIN);

		else /*if (pNavimeshTool->m_CellType[1].GetCheck() == TRUE)*/
			CNaviMgr::GetInstance()->Add_Cell(&m_vPoint[0], &m_vPoint[1], &m_vPoint[2], TYPE_MESH);

		pNavimeshTool->m_PointAx = m_vPoint[0].x;
		pNavimeshTool->m_PointAy = m_vPoint[0].y;
		pNavimeshTool->m_PointAz = m_vPoint[0].z;

		pNavimeshTool->m_PointBx = m_vPoint[1].x;
		pNavimeshTool->m_PointBy = m_vPoint[1].y;
		pNavimeshTool->m_PointBz = m_vPoint[1].z;

		pNavimeshTool->m_PointCx = m_vPoint[2].x;
		pNavimeshTool->m_PointCy = m_vPoint[2].y;
		pNavimeshTool->m_PointCz = m_vPoint[2].z;

		pNavimeshTool->m_iCellNum = CNaviMgr::GetInstance()->GetCell()->back()->GetIndex();

		CNaviMgr::GetInstance()->Link_Cell();

		ZeroMemory(m_vPoint, sizeof(D3DXVECTOR3) * 3);
	}

}

void CBack::ComparePoint(D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2, D3DXVECTOR3& vPoint3)
{
	D3DXVECTOR3		vPoint[3];

	vPoint[0] = ((vPoint1.x < vPoint2.x) ? vPoint1 : vPoint2);
	vPoint[0] = ((vPoint[0].x < vPoint3.x) ? vPoint[0] : vPoint3);

	D3DXVECTOR3		vCompareXPoint[2];
	if (vPoint[0] == vPoint1)
	{
		vCompareXPoint[0] = vPoint2;
		vCompareXPoint[1] = vPoint3;
	}
	else if (vPoint[0] == vPoint2)
	{
		vCompareXPoint[0] = vPoint1;
		vCompareXPoint[1] = vPoint3;
	}
	else
	{
		vCompareXPoint[0] = vPoint1;
		vCompareXPoint[1] = vPoint2;
	}

	vPoint[1] = ((vCompareXPoint[0].z > vCompareXPoint[1].z) ? vCompareXPoint[0] : vCompareXPoint[1]);
	vPoint[2] = ((vCompareXPoint[0].z < vCompareXPoint[1].z) ? vCompareXPoint[0] : vCompareXPoint[1]);




	vPoint1 = vPoint[0];
	vPoint2 = vPoint[1];
	vPoint3 = vPoint[2];
}

