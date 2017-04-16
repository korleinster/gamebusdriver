#include "stdafx.h"
#include "Node.h"
#include "Include.h"
#include "Frustum.h"
#include "Camera.h"

CNode::CNode(void)
{
	m_dwCenter = 0;
	m_fRadius = 0.f;

	ZeroMemory(m_dwCorner, sizeof(DWORD) * CORNER_END);
	ZeroMemory(m_pChildNode, sizeof(CNode*) * NODE_END);

	for (DWORD i = 0; i < CENTER_END; ++i)
	{
		m_dwNeighborCenter[i] = MAX;
	}
}

CNode::~CNode()
{

}

void CNode::Init_Node(const D3DXVECTOR3* pVtxPos, const DWORD& dwLT, const DWORD& dwRT, const DWORD& dwLB, const DWORD& dwRB)
{
	m_dwCorner[CORNER_LT] = dwLT;
	m_dwCorner[CORNER_RT] = dwRT;
	m_dwCorner[CORNER_LB] = dwLB;
	m_dwCorner[CORNER_RB] = dwRB;
	m_pVtxPos = pVtxPos;

	if (dwRT - dwLT <= 1)
		return;

	DWORD		dwLC, dwTC, dwRC, dwBC, dwCenter;
	dwLC = (dwLT + dwLB) >> 1;
	dwTC = (dwLT + dwRT) >> 1;
	dwRC = (dwRT + dwRB) >> 1;
	dwBC = (dwLB + dwRB) >> 1;
	m_dwCenter = dwCenter = (dwLB + dwRT) >> 1;

	m_fRadius = D3DXVec3Length(&(m_pVtxPos[m_dwCenter] - m_pVtxPos[m_dwCorner[CORNER_LT]]));

	m_pChildNode[NODE_LT] = CNode::Create(pVtxPos, dwLT, dwTC, dwLC, dwCenter);
	m_pChildNode[NODE_RT] = CNode::Create(pVtxPos, dwTC, dwRT, dwCenter, dwRC);
	m_pChildNode[NODE_LB] = CNode::Create(pVtxPos, dwLC, dwCenter, dwLB, dwBC);
	m_pChildNode[NODE_RB] = CNode::Create(pVtxPos, dwCenter, dwRC, dwBC, dwRB);
}

void CNode::Decision_In(CFrustum* pFrustum, INDEX32* pIndex, int* pTriCnt)
{
	if (LevelofDetail()
		|| NULL == m_pChildNode[NODE_LT])
	{
		bool 	bIn[3] = { false };

		// 절두체 안에있는 쿼드트리의 자식이면서 최하위 쿼드트리인 놈
		DWORD dwLC, dwTC, dwRC, dwBC;

		dwLC = DWORD((m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) * 0.5f);
		dwTC = DWORD((m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) * 0.5f);
		dwRC = DWORD((m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) * 0.5f);
		dwBC = DWORD((m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) * 0.5f);

		bool	isNeighborDraw[4] = { true, true, true, true };

		if (m_dwNeighborCenter[CENTER_TOP] != MAX)
			isNeighborDraw[0] = LevelofDetail(m_dwNeighborCenter[CENTER_TOP]);
		if (m_dwNeighborCenter[CENTER_RIGHT] != MAX)
			isNeighborDraw[1] = LevelofDetail(m_dwNeighborCenter[CENTER_RIGHT]);
		if (m_dwNeighborCenter[CENTER_LEFT] != MAX)
			isNeighborDraw[2] = LevelofDetail(m_dwNeighborCenter[CENTER_LEFT]);
		if (m_dwNeighborCenter[CENTER_BOTTOM] != MAX)
			isNeighborDraw[3] = LevelofDetail(m_dwNeighborCenter[CENTER_BOTTOM]);

		if (true == isNeighborDraw[0]
			&& true == isNeighborDraw[1]
			&& true == isNeighborDraw[2]
			&& true == isNeighborDraw[3])
		{
			bIn[0] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LT]]);
			bIn[1] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RT]]);
			bIn[2] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RB]]);

			if (bIn[0] || bIn[1] || bIn[2])
			{
				pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
				pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_RT];
				pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}

			bIn[0] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LT]]);
			bIn[1] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RB]]);
			bIn[2] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LB]]);

			if (bIn[0] || bIn[1] || bIn[2])
			{
				pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
				pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_RB];
				pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}
			return;
		}
		else
		{
			bIn[0] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LT]]);
			bIn[1] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RT]]);
			bIn[2] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RB]]);

			if (bIn[0] || bIn[1] || bIn[2])
			{
				if (true == isNeighborDraw[0])
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_RT];
					pIndex[*pTriCnt]._3 = (WORD)m_dwCenter;
					++(*pTriCnt);
				}
				else
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
					pIndex[*pTriCnt]._2 = (WORD)dwTC;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCenter;
					++(*pTriCnt);

					pIndex[*pTriCnt]._1 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._2 = (WORD)dwTC;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_RT];
					++(*pTriCnt);
				}

				if (true == isNeighborDraw[1])
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_RT];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_RB];
					pIndex[*pTriCnt]._3 = (WORD)m_dwCenter;
					++(*pTriCnt);
				}
				else
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_RT];
					pIndex[*pTriCnt]._2 = (WORD)dwRC;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCenter;
					++(*pTriCnt);

					pIndex[*pTriCnt]._1 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._2 = (WORD)dwRC;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_RB];
					++(*pTriCnt);
				}
			}

			bIn[0] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LT]]);
			bIn[1] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_RB]]);
			bIn[2] = pFrustum->Decision_In(&m_pVtxPos[m_dwCorner[CORNER_LB]]);

			if (bIn[0] || bIn[1] || bIn[2])
			{
				if (true == isNeighborDraw[2])
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_LB];
					++(*pTriCnt);
				}
				else
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LT];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._3 = (WORD)dwLC;
					++(*pTriCnt);

					pIndex[*pTriCnt]._1 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_LB];
					pIndex[*pTriCnt]._3 = (WORD)dwLC;
					++(*pTriCnt);
				}

				if (true == isNeighborDraw[3])
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LB];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._3 = (WORD)m_dwCorner[CORNER_RB];
					++(*pTriCnt);
				}
				else
				{
					pIndex[*pTriCnt]._1 = (WORD)m_dwCorner[CORNER_LB];
					pIndex[*pTriCnt]._2 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._3 = (WORD)dwBC;
					++(*pTriCnt);

					pIndex[*pTriCnt]._1 = (WORD)m_dwCenter;
					pIndex[*pTriCnt]._2 = (WORD)m_dwCorner[CORNER_RB];
					pIndex[*pTriCnt]._3 = (WORD)dwBC;
					++(*pTriCnt);
				}
			}
			return;
		}

	}

	if (true == pFrustum->Decision_In(&m_pVtxPos[m_dwCenter], m_fRadius))
	{
		for (DWORD i = 0; i < NODE_END; ++i)
		{
			if (NULL != m_pChildNode[i])
				m_pChildNode[i]->Decision_In(pFrustum, pIndex, pTriCnt);
		}
	}
}

void CNode::SetUp_NeighborCenterIdx(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ)
{
	if (NULL == m_pChildNode[0])
		return;

	Compute_NeighborCenter(dwVtxCntX, dwVtxCntZ);

	for (int i = 0; i < NODE_END; ++i)
	{
		if (NULL != m_pChildNode[i])
			m_pChildNode[i]->SetUp_NeighborCenterIdx(dwVtxCntX, dwVtxCntZ);
	}
}

CNode* CNode::Create(const D3DXVECTOR3* pVtxPos, const DWORD& dwLT, const DWORD& dwRT, const DWORD& dwLB, const DWORD& dwRB)
{
	CNode*		pNode = new CNode;

	pNode->Init_Node(pVtxPos, dwLT, dwRT, dwLB, dwRB);

	return pNode;
}

bool CNode::LevelofDetail(void)
{
	D3DXMATRIX			matView;
	matView = CCamera::GetInstance()->m_matView;
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVECTOR3		vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(D3DXVECTOR3));

	float		fDistance[2];
	fDistance[0] = D3DXVec3Length(&(m_pVtxPos[m_dwCenter] - vCamPos));
	fDistance[1] = m_pVtxPos[m_dwCorner[CORNER_RT]].x - m_pVtxPos[m_dwCorner[CORNER_LT]].x;

	if (fDistance[0] * 0.2f < fDistance[1])
		return false;

	return true;
}

bool CNode::LevelofDetail(const DWORD& dwCenter)
{
	D3DXMATRIX			matView;
	matView = CCamera::GetInstance()->m_matView;
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVECTOR3		vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(D3DXVECTOR3));

	float		fDistance[2];
	fDistance[0] = D3DXVec3Length(&(m_pVtxPos[dwCenter] - vCamPos));
	fDistance[1] = m_pVtxPos[m_dwCorner[CORNER_RT]].x - m_pVtxPos[m_dwCorner[CORNER_LT]].x;

	if (fDistance[0] * 0.2f < fDistance[1])
		return false;

	return true;
}

void CNode::Compute_NeighborCenter(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ)
{
	DWORD			dwNeighborIndex = 0;

	//  왼쪽이웃구하기
	dwNeighborIndex = m_dwCenter - (m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT]);
	if (m_dwCenter / dwVtxCntX == dwNeighborIndex / dwVtxCntX)
		m_dwNeighborCenter[CENTER_LEFT] = dwNeighborIndex;

	//  오른쪽이웃구하기
	dwNeighborIndex = m_dwCenter + (m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT]);

	if (m_dwCenter / dwVtxCntX == dwNeighborIndex / dwVtxCntX)
		m_dwNeighborCenter[CENTER_RIGHT] = dwNeighborIndex;

	//  위쪽이웃구하기
	dwNeighborIndex = m_dwCenter + (m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT]) * dwVtxCntX;
	if (dwNeighborIndex >= 0
		&& dwNeighborIndex < dwVtxCntX * dwVtxCntZ)
		m_dwNeighborCenter[CENTER_TOP] = dwNeighborIndex;

	//  아래쪽이웃구하기
	dwNeighborIndex = m_dwCenter - (m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT]) * dwVtxCntX;
	if (dwNeighborIndex >= 0
		&& dwNeighborIndex < dwVtxCntX * dwVtxCntZ)
		m_dwNeighborCenter[CENTER_BOTTOM] = dwNeighborIndex;
}

void CNode::Release(void)
{
	for (DWORD i = 0; i < NODE_END; ++i)
	{
		::Safe_Release(m_pChildNode[i]);
	}
}

