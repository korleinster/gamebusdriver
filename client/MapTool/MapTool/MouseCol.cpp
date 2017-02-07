#include "stdafx.h"
//#include "MouseCol.h"
//
//#include "Export_Function.h"
//#include "Include.h"
//
//#include "Transform.h"
//#include "MainFrm.h"
//#include "GameObject.h"
//#include "Mesh.h"
//#include "Component.h"
//#include "NaviCell.h"
//
//CMouseCol::CMouseCol(void)
////: m_pInfoSubject(Engine::Get_InfoSubject())
////, m_pCamObserver(NULL)
//	: m_iCntX(0)
//	, m_iCntZ(0)
//	, m_iItv(0)
//{
//
//}
//
//
//CMouseCol::~CMouseCol(void)
//{
//	Release();
//}
//
//HRESULT CMouseCol::Initialize(void)
//{
//	return S_OK;
//}
//
//void CMouseCol::Translation_ViewSpace(void)
//{
//	POINT	ptMouse = GetMousePos();
//
//	D3DXMATRIX	matProj;
//	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
//	D3DXVECTOR3	vTemp;
//
//	// 뷰 포트 영역에서 투영 역으로 마우스 변환
//	vTemp.x = ((float(ptMouse.x) / (WINCX >> 1)) - 1.f) / matProj._11;
//	vTemp.y = ((float(-ptMouse.y) / (WINCY >> 1)) + 1.f) / matProj._22;
//	vTemp.z = 1.f;
//
//	m_vPivotPos = D3DXVECTOR3(0.f, 0.f, 0.f);
//	m_CurMPos = D3DXVECTOR3(0.f, 0.f, 0.f);
//	m_vRayDir = vTemp - m_vPivotPos;
//
//	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
//}
//
//bool CMouseCol::Translation_ViewSpace2(void)
//{
//	POINT	ptMouse = GetMousePos();
//
//	if (ptMouse.x < 0 || ptMouse.y > 600)
//		return false;
//
//	D3DXMATRIX	matProj;
//	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
//	D3DXVECTOR3	vTemp;
//
//	// 뷰 포트 영역에서 투영 역으로 마우스 변환
//	vTemp.x = ((float(ptMouse.x) / (WINCX >> 1)) - 1.f) / matProj._11;
//	vTemp.y = ((float(-ptMouse.y) / (WINCY >> 1)) + 1.f) / matProj._22;
//	vTemp.z = 1.f;
//
//	m_vPivotPos = D3DXVECTOR3(0.f, 0.f, 0.f);
//
//	m_vRayDir = vTemp - m_vPivotPos;
//
//	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
//
//	return true;
//
//}
//
//
//void CMouseCol::Translation_Local(const D3DXMATRIX* pmatWolrd)
//{
//	D3DXMATRIX	matView;
//	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
//
//	D3DXMatrixInverse(&matView, 0, &matView);
//	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &matView);
//	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matView);
//
//	D3DXMATRIX		matWorld;
//	D3DXMatrixInverse(&matWorld, 0, pmatWolrd);
//	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &matWorld);
//	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matWorld);
//}
//
//POINT CMouseCol::GetMousePos(void)
//{
//	POINT		ptMouse;
//
//	GetCursorPos(&ptMouse);
//	ScreenToClient(g_hWnd, &ptMouse);
//
//	return ptMouse;
//}
//
//void CMouseCol::PickTerrain(D3DXVECTOR3* pOut, const Engine::VTXTEX* pTerrainVtx)
//{
//	Translation_ViewSpace();
//
//	D3DXMATRIX		matIdentity;
//	D3DXMatrixIdentity(&matIdentity);
//	Translation_Local(&matIdentity);
//
//	const Engine::VTXTEX* pVertex = pTerrainVtx;
//
//	float	fU, fV, fDist;
//
//	for (int z = 0; z < m_iCntZ - 1; ++z)
//	{
//		for (int x = 0; x < m_iCntX - 1; ++x)
//		{
//			int		iIndex = z * m_iCntX + x;
//
//			// 오른쪽 위 폴리곤
//			if (D3DXIntersectTri(&pVertex[iIndex + m_iCntX + 1].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				*pOut = pVertex[iIndex + m_iCntX + 1].vPos +
//					(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fU +
//					(pVertex[iIndex + 1].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fV;
//
//				return;
//			}
//
//			// 
//			if (D3DXIntersectTri(&pVertex[iIndex].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				*pOut = pVertex[iIndex].vPos +
//					(pVertex[iIndex + 1].vPos - pVertex[iIndex].vPos) * fV +
//					(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex].vPos) * fU;
//
//				return;
//			}
//		}
//	}
//}
//
//void CMouseCol::PickTerrainVertex(D3DXVECTOR3* pOut, const Engine::VTXTEX* pTerrainVtx)
//{
//	// 맵툴에서 점 피킹
//	Translation_ViewSpace();
//
//	D3DXMATRIX		matIdentity;
//	D3DXMatrixIdentity(&matIdentity);
//	Translation_Local(&matIdentity);
//
//	const Engine::VTXTEX* pVertex = pTerrainVtx;
//
//	float	fU, fV, fDist;
//
//	for (int z = 0; z < m_iCntZ - 1; ++z)
//	{
//		for (int x = 0; x < m_iCntX - 1; ++x)
//		{
//			int		iIndex = z * m_iCntX + x;
//
//			// 오른쪽 위 폴리곤
//			if (D3DXIntersectTri(&pVertex[iIndex + m_iCntX + 1].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				m_CurMPos = pVertex[iIndex + m_iCntX + 1].vPos +
//					(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fU +
//					(pVertex[iIndex + 1].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fV;
//
//				float fVtxDis[3] = { 0.f, };
//				D3DXVECTOR3 vDis[3] = { D3DXVECTOR3(0.f, 0.f, 0.f), };
//
//				vDis[0] = pVertex[iIndex + m_iCntX + 1].vPos - m_CurMPos;
//				vDis[1] = pVertex[iIndex + m_iCntX].vPos - m_CurMPos;
//				vDis[2] = pVertex[iIndex + 1].vPos - m_CurMPos;
//				for (int i = 0; i < 3; ++i)
//				{
//					fVtxDis[i] = D3DXVec3Length(&vDis[i]);
//				}
//				float fMaxDis = min(fVtxDis[0], fVtxDis[1]);
//				fMaxDis = min(fMaxDis, fVtxDis[2]);
//				int iTemp;
//				for (iTemp = 0; iTemp < 3; ++iTemp)
//				{
//					if (fMaxDis == fVtxDis[iTemp])
//						break;
//				}
//				switch (iTemp)
//				{
//				case 0:
//					*pOut = pVertex[iIndex + m_iCntX + 1].vPos;
//					break;
//				case 1:
//					*pOut = pVertex[iIndex + m_iCntX].vPos;
//					break;
//				case 2:
//					*pOut = pVertex[iIndex + 1].vPos;
//					break;
//				}
//				/**pOut	 = pVertex[iIndex + m_iCntX + 1].vPos +
//				(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fU +
//				(pVertex[iIndex + 1].vPos - pVertex[iIndex + m_iCntX + 1].vPos) * fV;*/
//
//				return;
//			}
//
//			// 
//			if (D3DXIntersectTri(&pVertex[iIndex].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				m_CurMPos = pVertex[iIndex].vPos +
//					(pVertex[iIndex + 1].vPos - pVertex[iIndex].vPos) * fV +
//					(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex].vPos) * fU;
//
//				float fVtxDis[3] = { 0.f, };
//				D3DXVECTOR3 vDis[3] = { D3DXVECTOR3(0.f, 0.f, 0.f), };
//
//				vDis[0] = pVertex[iIndex].vPos - m_CurMPos;
//				vDis[1] = pVertex[iIndex + m_iCntX].vPos - m_CurMPos;
//				vDis[2] = pVertex[iIndex + 1].vPos - m_CurMPos;
//				for (int i = 0; i < 3; ++i)
//				{
//					fVtxDis[i] = D3DXVec3Length(&vDis[i]);
//				}
//				float fMaxDis = min(fVtxDis[0], fVtxDis[1]);
//				fMaxDis = min(fMaxDis, fVtxDis[2]);
//				int iTemp;
//				for (iTemp = 0; iTemp < 3; ++iTemp)
//				{
//					if (fMaxDis == fVtxDis[iTemp])
//						break;
//				}
//				switch (iTemp)
//				{
//				case 0:
//					*pOut = pVertex[iIndex].vPos;
//					break;
//				case 1:
//					*pOut = pVertex[iIndex + m_iCntX].vPos;
//					break;
//				case 2:
//					*pOut = pVertex[iIndex + 1].vPos;
//					break;
//				}
//				/**pOut	 = pVertex[iIndex].vPos +
//				(pVertex[iIndex + 1].vPos - pVertex[iIndex].vPos) * fU +
//				(pVertex[iIndex + m_iCntX].vPos - pVertex[iIndex].vPos) * fV;*/
//
//				return;
//			}
//		}
//	}
//}
//
//bool CMouseCol::PickTerrainIndex(D3DXVECTOR3* pOut, const Engine::VTXTEX* pTerrainVtx)
//{	// 찍은부분 좌표를 보내줌
//	Translation_ViewSpace();
//
//	D3DXMATRIX		matIdentity;
//	D3DXMatrixIdentity(&matIdentity);
//	Translation_Local(&matIdentity);
//
//	const Engine::VTXTEX* pVertex = pTerrainVtx;
//
//	float	fU, fV, fDist;
//
//	for (int z = 0; z < m_iCntZ - 1; ++z)
//	{
//		for (int x = 0; x < m_iCntX - 1; ++x)
//		{
//			int		iIndex = z * m_iCntX + x;
//
//			// 오른쪽 위 폴리곤
//			if (D3DXIntersectTri(&pVertex[iIndex + m_iCntX + 1].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				*pOut = D3DXVECTOR3((float)x, 0.f, (float)z);
//
//				return true;
//			}
//
//			// 왼쪽 아래
//			if (D3DXIntersectTri(&pVertex[iIndex].vPos,
//				&pVertex[iIndex + m_iCntX].vPos,
//				&pVertex[iIndex + 1].vPos,
//				&m_vPivotPos, &m_vRayDir,
//				&fU, &fV, &fDist))
//			{
//				*pOut = D3DXVECTOR3((float)x, 0.f, (float)z);
//
//				return true;
//			}
//		}
//	}
//	return false;
//}
//
//bool CMouseCol::PickObject(D3DXVECTOR3* pOut, const Engine::VTXTEX* pVertex, const D3DXMATRIX* pmatWorld)
//{
//	Translation_ViewSpace();
//	Translation_Local(pmatWorld);
//
//	const Engine::VTXTEX* pVertex2 = pVertex;
//
//	float	fU, fV, fDist;
//
//	// 오른쪽 위 폴리곤
//	if (D3DXIntersectTri(&pVertex2[1].vPos,
//		&pVertex2[0].vPos,
//		&pVertex2[2].vPos,
//		&m_vPivotPos, &m_vRayDir,
//		&fU, &fV, &fDist))
//	{
//		*pOut = pVertex2[1].vPos +
//			(pVertex2[0].vPos - pVertex2[1].vPos) * fU +
//			(pVertex2[2].vPos - pVertex2[1].vPos) * fV;
//
//		return true;
//	}
//	// 왼쪽 아래 폴리곤
//	if (D3DXIntersectTri(&pVertex2[3].vPos,
//		&pVertex2[0].vPos,
//		&pVertex2[2].vPos,
//		&m_vPivotPos, &m_vRayDir,
//		&fU, &fV, &fDist))
//	{
//		*pOut = pVertex2[3].vPos +
//			(pVertex2[0].vPos - pVertex2[1].vPos) * fU +
//			(pVertex2[2].vPos - pVertex2[1].vPos) * fV;
//
//		return true;
//	}
//
//	return false;
//}
//
//bool CMouseCol::PickObject(D3DXVECTOR3* pOut, Engine::VTXCOL* pVertex, Engine::INDEX16* pIndex, const D3DXMATRIX* pmatWorld)
//{
//	Translation_ViewSpace();
//	Translation_Local(pmatWorld);
//
//	float	fU, fV, fDist;
//	size_t i = 0;
//	for (i; i < 12; ++i)
//	{
//		// 오른쪽 위 폴리곤
//		if (D3DXIntersectTri(&pVertex[pIndex[i]._2].vPos,
//			&pVertex[pIndex[i]._1].vPos,
//			&pVertex[pIndex[i]._3].vPos,
//			&m_vPivotPos, &m_vRayDir,
//			&fU, &fV, &fDist))
//		{
//			*pOut = pVertex[pIndex[i]._2].vPos +
//				(pVertex[pIndex[i]._1].vPos - pVertex[pIndex[i]._2].vPos) * fU +
//				(pVertex[pIndex[i]._3].vPos - pVertex[pIndex[i]._2].vPos) * fV;
//
//			break;
//		}
//		++i;
//		// 왼쪽 아래 폴리곤
//		if (D3DXIntersectTri(&pVertex[pIndex[i]._3].vPos,
//			&pVertex[pIndex[i]._2].vPos,
//			&pVertex[pIndex[i]._1].vPos,
//			&m_vPivotPos, &m_vRayDir,
//			&fU, &fV, &fDist))
//		{
//			*pOut = pVertex[pIndex[i]._3].vPos +
//				(pVertex[pIndex[i]._2].vPos - pVertex[pIndex[i]._3].vPos) * fU +
//				(pVertex[pIndex[i]._1].vPos - pVertex[pIndex[i]._3].vPos) * fV;
//
//			break;
//		}
//	}
//
//	if (i != 12)
//		return true;
//
//	return false;
//}
//
//void CMouseCol::Release(void)
//{
//
//}
//
//void CMouseCol::SetSize(int iCntX, int iCntZ, int iItv)
//{
//	m_iCntX = iCntX;
//	m_iCntZ = iCntZ;
//	m_iItv = iItv;
//}
//bool CMouseCol::MeshPick(D3DXVECTOR3* pOut, LPD3DXMESH pMesh, D3DXMATRIX maWorld)
//{
//	if (Translation_ViewSpace2() == false)
//	{
//		return false;
//	}
//
//	D3DXMATRIX		matIdentity;
//	D3DXMatrixIdentity(&matIdentity);
//	Translation_Local(&maWorld);
//
//	BOOL bHit;
//
//	float fDir;
//	float fU;
//	float fV;
//	DWORD dwFaceIndex;
//	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
//
//	D3DXIntersect(pMesh, &m_vPivotPos, &m_vRayDir, &bHit, &dwFaceIndex, &fU, &fV, &fDir, NULL, NULL);
//
//	if (bHit == TRUE)
//	{
//		*pOut = m_vPivotPos + m_vRayDir * fDir;
//		/*pOut->x = pOut->x * maWorld._11;
//		pOut->y = pOut->y * maWorld._22;
//		pOut->z = pOut->z * maWorld._33;*/
//
//		return true;
//	}
//	else
//		return false;
//}
//
////bool CMouseCol::PickNaviCell(D3DXVECTOR3* pOut, CNaviCell* pCell, int* pOutIndex)
////{
////	Translation_ViewSpace();
////	D3DXMATRIX matIdentity;
////	D3DXMatrixIdentity(&matIdentity);
////	Translation_Local(&matIdentity);
////
////	const D3DXVECTOR3* vPoint1 = (*pCell).GetPoint(Engine::CNaviCell::POINT_A);
////	const D3DXVECTOR3* vPoint2 = (*pCell).GetPoint(Engine::CNaviCell::POINT_B);
////	const D3DXVECTOR3* vPoint3 = (*pCell).GetPoint(Engine::CNaviCell::POINT_C);
////
////	float fU, fV, fDist;
////
////	int iSize = Get_Cell().size();
////
////	for (int z = 0; z < iSize; ++z)
////	{
////		if (D3DXIntersectTri(vPoint1, vPoint2, vPoint3, &m_vPivotPos, &m_vRayDir, &fU, &fV, &fDist))
////		{
////			*pOut = *vPoint1 + (*vPoint2 - *vPoint1) * fU + (*vPoint3 - *vPoint1) * fV;
////			*pOutIndex = pCell->GetIndex();
////			return true;
////		}
////	}
////
////
////	*pOut = D3DXVECTOR3(0.f, 0.f, 0.f);
////	*pOutIndex = NULL;
////	return false;
////}
//
////bool CMouseCol::PickOccBox(D3DXVECTOR3* pOut, Engine::VTXCOL* pVertex, Engine::INDEX16* pIndex, const D3DXMATRIX* pmatWorld)
////{
////	Translation_ViewSpace();
////	Translation_Local(pmatWorld);
////
////	float	fU, fV, fDist;
////	size_t i = 0;
////	for (i; i < 12; ++i)
////	{
////		// 오른쪽 위 폴리곤
////		if (D3DXIntersectTri(&pVertex[pIndex[i]._2].vPos,
////			&pVertex[pIndex[i]._1].vPos,
////			&pVertex[pIndex[i]._3].vPos,
////			&m_vPivotPos, &m_vRayDir,
////			&fU, &fV, &fDist))
////		{
////			*pOut = pVertex[pIndex[i]._2].vPos +
////				(pVertex[pIndex[i]._1].vPos - pVertex[pIndex[i]._2].vPos) * fU +
////				(pVertex[pIndex[i]._3].vPos - pVertex[pIndex[i]._2].vPos) * fV;
////
////			break;
////		}
////		++i;
////		// 왼쪽 아래 폴리곤
////		if (D3DXIntersectTri(&pVertex[pIndex[i]._3].vPos,
////			&pVertex[pIndex[i]._2].vPos,
////			&pVertex[pIndex[i]._1].vPos,
////			&m_vPivotPos, &m_vRayDir,
////			&fU, &fV, &fDist))
////		{
////			*pOut = pVertex[pIndex[i]._3].vPos +
////				(pVertex[pIndex[i]._2].vPos - pVertex[pIndex[i]._3].vPos) * fU +
////				(pVertex[pIndex[i]._1].vPos - pVertex[pIndex[i]._3].vPos) * fV;
////
////			break;
////		}
////	}
////
////	if (i != 12)
////		return true;
////
////	return false;
////}
