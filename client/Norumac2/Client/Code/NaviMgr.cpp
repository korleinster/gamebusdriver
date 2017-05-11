#include "stdafx.h"
#include "NaviMgr.h"
#include "Include.h"
#include "LineCol.h"
#include "NaviCell.h"
#include "Line2D.h"


IMPLEMENT_SINGLETON(CNaviMgr)

CNaviMgr::CNaviMgr(void)
	:m_dwReserveSize(0)
	, m_dwIdxCnt(0)
{

}

CNaviMgr::~CNaviMgr(void)
{

}

HRESULT CNaviMgr::Reserve_CellContainerSize(const DWORD& dwSize)
{
	if (m_dwReserveSize != 0 || m_vecNaviMesh.size() != 0)
		return E_FAIL;

	m_dwReserveSize = dwSize;
	m_vecNaviMesh.reserve(dwSize);

	return S_OK;
}

void CNaviMgr::Release(void)
{
	for_each(m_vecNaviMesh.begin(), m_vecNaviMesh.end(), CDeleteObj());
	m_vecNaviMesh.clear();
}

HRESULT CNaviMgr::Add_Cell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, const DWORD& dwType)
{
	if (m_dwReserveSize == 0)
		return E_FAIL;

	CNaviCell*		pCell = CNaviCell::Create(pPointA, pPointB, pPointC, m_dwIdxCnt, dwType);
	if (pCell == NULL)
		return E_FAIL;

	++m_dwIdxCnt;

	m_vecNaviMesh.push_back(pCell);

	CLineCol* pLineCol = CLineCol::Create(pCell);
	if (pLineCol == NULL)
		return E_FAIL;

	pCell->AddComponent(pLineCol);


	return S_OK;
}


void CNaviMgr::Render(void)
{
	UINT		iSize = m_vecNaviMesh.size();

	for (UINT i = 0; i < iSize; ++i)
	{
		m_vecNaviMesh[i]->Render();
	}
}
void CNaviMgr::Link_Cell(void)
{
	VECCELL::iterator	iter = m_vecNaviMesh.begin();

	if (m_vecNaviMesh.end() == iter)
		return;

	for (; iter != m_vecNaviMesh.end(); ++iter)
	{
		VECCELL::iterator	iter_Target = m_vecNaviMesh.begin();

		while (iter_Target != m_vecNaviMesh.end())
		{
			if (iter == iter_Target)
			{
				++iter_Target;
				continue;
			}

			if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_A)
				, (*iter)->GetPoint(CNaviCell::POINT_B), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_AB, (*iter_Target));
			}
			else if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_B)
				, (*iter)->GetPoint(CNaviCell::POINT_C), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_BC, (*iter_Target));
			}
			else if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_C)
				, (*iter)->GetPoint(CNaviCell::POINT_A), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_CA, (*iter_Target));
			}

			++iter_Target;
		}

	}
}
DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwCurrentIdx)
{
	CNaviCell::NEIGHBOR		Neighbor;
	DWORD		dwNextIndex = dwCurrentIdx;

	D3DXVECTOR3 vBefore = *pPos;

	bool		bPassNeighbor[CNaviCell::NEIGHBOR_END] = { false, false, false };
	for (int i = 0; i < CNaviCell::NEIGHBOR_END; ++i)
	{
		bPassNeighbor[i] = m_vecNaviMesh[dwCurrentIdx]->CheckPass(pPos, pDir, (DWORD)i);
	}
	int iCnt = 0;
	for (int i = 0; i < CNaviCell::NEIGHBOR_END; ++i)
	{
		if (bPassNeighbor[i])
			++iCnt;
	}

	if (m_vecNaviMesh[dwCurrentIdx]->CheckPass(pPos, pDir, &Neighbor))
	{
		CNaviCell*		pNeighbor = m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(Neighbor);
		if (pNeighbor == NULL)
		{
			// 슬라이딩 벡터
			if (iCnt == 1)
			{
				/*D3DXVECTOR3 vDir = *(const_cast<D3DXVECTOR3*>(pDir));

				vDir = m_vecNaviMesh[dwCurrentIdx]->SlidingVector(vDir, Neighbor);
				*pPos += vDir;*/

				D3DXVECTOR3 vDir = *(const_cast<D3DXVECTOR3*>(pDir));

				D3DXVECTOR3 vPoint[3];
				D3DXVECTOR3 vLine[2];
				D3DXVECTOR3 vCrossLine, vPlane;
				float fY;

				D3DXVec3Normalize(&vCrossLine, &vCrossLine);
				D3DXVec3Normalize(&vPlane, &vPlane);
				vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
				vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
				vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

				vLine[0] = vPoint[0] - vPoint[1];
				vLine[1] = vPoint[0] - vPoint[2];

				D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

				fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;

				vDir = m_vecNaviMesh[dwCurrentIdx]->SlidingVector(vDir, Neighbor);
				*pPos += vDir;
				pPos->y = fY;

			}
			else if (iCnt == 2)
			{
				D3DXVECTOR2 vPos = D3DXVECTOR2((*pPos).x, (*pPos).z);
				D3DXVECTOR3 vDir = *(const_cast<D3DXVECTOR3*>(pDir));
				vDir = m_vecNaviMesh[dwCurrentIdx]->SlidingVector(vDir, Neighbor);

				if (Neighbor == CNaviCell::NEIGHBOR_AB)
				{
					D3DXVECTOR2 vA = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_AB)->m_vStartPoint - vPos;
					D3DXVECTOR2 vB = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_AB)->m_vEndPoint - vPos;
					float fLengthA = D3DXVec2Length(&vA);
					float fLengthB = D3DXVec2Length(&vB);
					bool bCompare = true;
					if (fLengthA > fLengthB)
						bCompare = false;

					if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_BC) && bCompare == false)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
					else if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_CA) && bCompare == true)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
				}
				else if (Neighbor == CNaviCell::NEIGHBOR_BC)
				{
					D3DXVECTOR2 vB = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_BC)->m_vStartPoint - vPos;
					D3DXVECTOR2 vC = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_BC)->m_vEndPoint - vPos;
					float fLengthB = D3DXVec2Length(&vB);
					float fLengthC = D3DXVec2Length(&vC);
					bool bCompare = true;
					if (fLengthB > fLengthC)
						bCompare = false;

					if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_CA) && bCompare == false)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
					else if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_AB) && bCompare == true)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
				}
				else if (Neighbor == CNaviCell::NEIGHBOR_CA)
				{
					D3DXVECTOR2 vC = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_CA)->m_vStartPoint - vPos;
					D3DXVECTOR2 vA = m_vecNaviMesh[dwCurrentIdx]->GetLine(CNaviCell::LINE_CA)->m_vEndPoint - vPos;
					float fLengthC = D3DXVec2Length(&vC);
					float fLengthA = D3DXVec2Length(&vA);
					bool bCompare = true;
					if (fLengthC > fLengthA)
						bCompare = false;

					if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_AB) && bCompare == false)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
					else if (NULL != m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(CNaviCell::NEIGHBOR_BC) && bCompare == true)
					{
						D3DXVECTOR3 vPoint[3];
						D3DXVECTOR3 vLine[2];
						D3DXVECTOR3 vCrossLine;
						float fY;

						D3DXVec3Normalize(&vCrossLine, &vCrossLine);
						vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
						vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
						vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

						vLine[0] = vPoint[0] - vPoint[1];
						vLine[1] = vPoint[0] - vPoint[2];

						D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

						fY = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
						pPos->y = fY;
						*pPos += vDir;
					}
				}
			}
			return dwCurrentIdx;
		}
		else
		{
			dwNextIndex = pNeighbor->GetIndex();

			D3DXVECTOR3 vPoint[3];
			D3DXVECTOR3 vLine[2];
			D3DXVECTOR3 vCrossLine;
			//float fY;

			D3DXVec3Normalize(&vCrossLine, &vCrossLine);
			vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
			vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
			vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

			vLine[0] = vPoint[0] - vPoint[1];
			vLine[1] = vPoint[0] - vPoint[2];

			D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);

			if (m_vecNaviMesh[dwCurrentIdx]->GetCellType() == TYPE_MESH)
			{
				pPos->y = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
			}
			else if (m_vecNaviMesh[dwCurrentIdx]->GetCellType() == TYPE_TERRAIN)
			{
				//pPos->y = fHeight;
			}
			*pPos += *pDir;
		}
	}
	else
	{
		D3DXVECTOR3 vPoint[3];
		D3DXVECTOR3 vLine[2];
		D3DXVECTOR3 vCrossLine;
		//float fY;

		D3DXVec3Normalize(&vCrossLine, &vCrossLine);
		vPoint[0] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_A);
		vPoint[1] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_B);
		vPoint[2] = *m_vecNaviMesh[dwCurrentIdx]->GetPoint(CNaviCell::POINT_C);

		vLine[0] = vPoint[0] - vPoint[1];
		vLine[1] = vPoint[0] - vPoint[2];
		D3DXVec3Cross(&vCrossLine, &vLine[0], &vLine[1]);



		if (m_vecNaviMesh[dwCurrentIdx]->GetCellType() == TYPE_MESH)
		{
			//fY = ( (-(pPos->x * vCrossLine.x )) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z) ) / vCrossLine.y;
			pPos->y = ((-(pPos->x * vCrossLine.x)) + (vPoint[0].x * vCrossLine.x) + (vPoint[0].y * vCrossLine.y) - (pPos->z * vCrossLine.z) + (vPoint[0].z * vCrossLine.z)) / vCrossLine.y;
		}
		else if (m_vecNaviMesh[dwCurrentIdx]->GetCellType() == TYPE_TERRAIN)
		{
			//pPos->y = fHeight;
		}
		*pPos += *pDir;
	}

	return dwNextIndex;
}

void CNaviMgr::Unlink_Cell(void)
{
	VECCELL::iterator	iter = m_vecNaviMesh.begin();

	while (iter != m_vecNaviMesh.end())
	{
		CNaviCell* pTemp = (*iter)->GetNeighbor(CNaviCell::NEIGHBOR_AB);
		pTemp = NULL;
		pTemp = (*iter)->GetNeighbor(CNaviCell::NEIGHBOR_BC);
		pTemp = NULL;
		pTemp = (*iter)->GetNeighbor(CNaviCell::NEIGHBOR_CA);
		pTemp = NULL;

		++iter;
	}
}

int CNaviMgr::GetSize(void)
{
	return m_vecNaviMesh.size();
}

bool CNaviMgr::SetOnePoint(D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2, D3DXVECTOR3 vPoint3)
{
	D3DXVECTOR3 vTemp = (*(m_vecNaviMesh[0]->GetPoint(CNaviCell::POINT_A))
		+ *(m_vecNaviMesh[0]->GetPoint(CNaviCell::POINT_B))
		+ *(m_vecNaviMesh[0]->GetPoint(CNaviCell::POINT_C))) / 3.f
		- vPoint3;
	CNaviCell* pNaviClose = m_vecNaviMesh[0];

	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		D3DXVECTOR3 vTemp2 = (*(m_vecNaviMesh[i]->GetPoint(CNaviCell::POINT_A))
			+ *(m_vecNaviMesh[i]->GetPoint(CNaviCell::POINT_B))
			+ *(m_vecNaviMesh[i]->GetPoint(CNaviCell::POINT_C))) / 3.f
			- vPoint3;

		pNaviClose = (D3DXVec3Length(&vTemp2) < D3DXVec3Length(&vTemp) ? m_vecNaviMesh[i] : pNaviClose);

		vTemp = (*(pNaviClose->GetPoint(CNaviCell::POINT_A))
			+ *(pNaviClose->GetPoint(CNaviCell::POINT_B))
			+ *(pNaviClose->GetPoint(CNaviCell::POINT_C))) / 3.f
			- vPoint3;
	}

	/*D3DXVECTOR3 vLength[4];
	vLength[0] = *(pNaviClose->GetPoint(CNaviCell::POINT_A)) - vPoint3;
	vLength[1] = *(pNaviClose->GetPoint(CNaviCell::POINT_B)) - vPoint3;
	vLength[2] = *(pNaviClose->GetPoint(CNaviCell::POINT_C)) - vPoint3;

	vPoint1 = (D3DXVec3Length(&vLength[0]) < D3DXVec3Length(&vLength[1]) ?
	*(pNaviClose->GetPoint(CNaviCell::POINT_A)) : *(pNaviClose->GetPoint(CNaviCell::POINT_B)));

	D3DXVECTOR3 vTempResult = (D3DXVec3Length(&vLength[0]) > D3DXVec3Length(&vLength[1]) ?
	*(pNaviClose->GetPoint(CNaviCell::POINT_A)) : *(pNaviClose->GetPoint(CNaviCell::POINT_B)));
	vLength[3] = vTempResult - vPoint3;

	vPoint2 = (D3DXVec3Length(&vLength[2]) < D3DXVec3Length(&vLength[3]) ?
	*(pNaviClose->GetPoint(CNaviCell::POINT_C)) : vTempResult);*/

	D3DXVECTOR2 vDir = D3DXVECTOR2(vPoint3.x, vPoint3.z);

	return SetTwoPoint(vDir, pNaviClose, vPoint1, vPoint2);
}

bool CNaviMgr::SetTwoPoint(D3DXVECTOR2 vDir, CNaviCell* pNaviClose, D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2)
{
	if (NULL != pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_AB)
		&& NULL != pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_BC)
		&& NULL != pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_CA))
		return false;

	if (CLine2D::DIR_LEFT == pNaviClose->GetLine(CNaviCell::LINE_AB)->Check_Dir(&vDir))
	{
		if (NULL == pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_AB))
		{
			vPoint1 = *(pNaviClose->GetPoint(CNaviCell::POINT_A));
			vPoint2 = *(pNaviClose->GetPoint(CNaviCell::POINT_B));
		}
		else
		{
			return SetTwoPoint(vDir, pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_AB), vPoint1, vPoint2);
		}

	}
	else if (CLine2D::DIR_LEFT == pNaviClose->GetLine(CNaviCell::LINE_BC)->Check_Dir(&vDir))
	{
		if (NULL == pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_BC))
		{
			vPoint1 = *(pNaviClose->GetPoint(CNaviCell::POINT_B));
			vPoint2 = *(pNaviClose->GetPoint(CNaviCell::POINT_C));
		}
		else
		{
			return SetTwoPoint(vDir, pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_BC), vPoint1, vPoint2);
		}
	}
	else if (CLine2D::DIR_LEFT == pNaviClose->GetLine(CNaviCell::LINE_CA)->Check_Dir(&vDir))
	{
		if (NULL == pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_CA))
		{
			vPoint1 = *(pNaviClose->GetPoint(CNaviCell::POINT_C));
			vPoint2 = *(pNaviClose->GetPoint(CNaviCell::POINT_A));
		}
		else
		{
			return SetTwoPoint(vDir, pNaviClose->GetNeighbor(CNaviCell::NEIGHBOR_CA), vPoint1, vPoint2);
		}
	}
	return true;
}

void CNaviMgr::GetNearPoint(D3DXVECTOR3& vPoint, float fRange)
{
	if (m_vecNaviMesh.empty())
		return;

	D3DXVECTOR3 vMinPt = *(m_vecNaviMesh[0]->GetPoint(CNaviCell::POINT_A)) - vPoint;
	float fMinDist = D3DXVec3Length(&vMinPt);

	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		for (int j = 0; j < CNaviCell::POINT_END; ++j)
		{
			D3DXVECTOR3 vTemp2 = *(m_vecNaviMesh[i]->GetPoint((CNaviCell::POINT)j)) - vPoint;
			float fTempDist = D3DXVec3Length(&vTemp2);

			vMinPt = (fMinDist < fTempDist ? vMinPt : vTemp2);
			fMinDist = min(fMinDist, fTempDist);
		}
	}

	if (fMinDist < fRange)
		vPoint += vMinPt;
	else
		return;
}

void CNaviMgr::ChangePoint(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour)
{
	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		for (int j = 0; j < CNaviCell::POINT_END; ++j)
		{
			if (*(m_vecNaviMesh[i]->GetPoint((CNaviCell::POINT)j)) == vSour)
				m_vecNaviMesh[i]->SetPoint(vDest, (CNaviCell::POINT)j);
		}
	}
}

void CNaviMgr::DeletePoint(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour)
{
	D3DXVECTOR3 vDist = vDest - vSour;
	if (D3DXVec3Length(&vDist) > 1.0)
		return;
	VECCELL::iterator iter = m_vecNaviMesh.begin();
	VECCELL::iterator iter_end = m_vecNaviMesh.end();

	for (iter; iter != iter_end;)
	{
		D3DXVECTOR3 vPointA = *((*iter)->GetPoint(CNaviCell::POINT_A));
		D3DXVECTOR3 vPointB = *((*iter)->GetPoint(CNaviCell::POINT_B));
		D3DXVECTOR3 vPointC = *((*iter)->GetPoint(CNaviCell::POINT_C));

		if (vPointA != vSour && vPointB != vSour && vPointC != vSour)
		{
			++iter;
			continue;
		}

		Safe_Release(*iter);
		m_vecNaviMesh.erase(iter);
		iter = m_vecNaviMesh.begin();
		iter_end = m_vecNaviMesh.end();
	}
}

void CNaviMgr::DeleteTriangle(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour)
{
	D3DXVECTOR3 vDist = vDest - vSour;
	if (D3DXVec3Length(&vDist) > 1.0)
		return;

	D3DXVECTOR2 vDir = D3DXVECTOR2(vDest.x, vDest.z);

	VECCELL::iterator iter = m_vecNaviMesh.begin();
	VECCELL::iterator iter_end = m_vecNaviMesh.end();

	for (iter; iter != iter_end;)
	{
		CLine2D::DIR eDirA = (*iter)->GetLine(CNaviCell::LINE_AB)->Check_Dir(&vDir);
		CLine2D::DIR eDirB = (*iter)->GetLine(CNaviCell::LINE_BC)->Check_Dir(&vDir);
		CLine2D::DIR eDirC = (*iter)->GetLine(CNaviCell::LINE_CA)->Check_Dir(&vDir);

		if (eDirA == CLine2D::DIR_LEFT || eDirB == CLine2D::DIR_LEFT || eDirC == CLine2D::DIR_LEFT)
		{
			++iter;
			continue;
		}

		Safe_Release(*iter);
		m_vecNaviMesh.erase(iter);
		iter = m_vecNaviMesh.begin();
		iter_end = m_vecNaviMesh.end();
		break;
	}
}

void CNaviMgr::ResetNavimesh(void)
{
	VECCELL::iterator iter = m_vecNaviMesh.begin();
	VECCELL::iterator iter_end = m_vecNaviMesh.end();

	for (iter; iter != iter_end; ++iter)
	{
		Safe_Release(*iter);
	}
	m_vecNaviMesh.clear();
}

void CNaviMgr::ResetIndex(void)
{
	DWORD dwTempIndex[50000];
	ZeroMemory(dwTempIndex, sizeof(DWORD) * 50000);

	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		dwTempIndex[i] = m_vecNaviMesh[i]->GetIndex();
	}

	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		for (size_t j = 0; j < m_vecNaviMesh.size(); ++j)
		{
			if (m_vecNaviMesh[i]->GetIndex() == dwTempIndex[j])
				m_vecNaviMesh[i]->SetIndex(DWORD(i));
		}
	}

}

DWORD CNaviMgr::GetCellIndex(D3DXVECTOR3* pPos)
{
	int iCellCnt = 0;

	VECCELL::iterator iter = m_vecNaviMesh.begin();
	VECCELL::iterator iter_end = m_vecNaviMesh.end();
	bool		bPassNeighbor[CNaviCell::NEIGHBOR_END] = { false, false, false };

	for (iter; iter != iter_end; ++iter)
	{
		for (int i = 0; i < CNaviCell::NEIGHBOR_END; ++i)
		{
			bPassNeighbor[i] = (*iter)->CheckPass(pPos, &(D3DXVECTOR3(0.f, 0.f, 0.f)), i);
			if (bPassNeighbor[i] == false)
				++iCellCnt;

			if (iCellCnt == 3)
				break;
		}

		if (iCellCnt == 3)
			break;
		else
			iCellCnt = 0;
	}
	if (iter == iter_end)
		return 0;

	return (*iter)->GetIndex();
}

DWORD CNaviMgr::GetCellType(DWORD dwIndex)
{
	return m_vecNaviMesh[dwIndex]->GetCellType();
}