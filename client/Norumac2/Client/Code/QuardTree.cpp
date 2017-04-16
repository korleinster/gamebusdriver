#include "stdafx.h"
#include "Include.h"
#include "Node.h"
#include "Frustum.h"
#include "QuardTree.h"

CQuardTree::CQuardTree(void)
{

}

CQuardTree::~CQuardTree(void)
{

}

HRESULT CQuardTree::Initialize_QuadTree(const D3DXVECTOR3* pVtxPos, const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ)
{
	m_pParentNode = CNode::Create(pVtxPos, dwVtxCntX * dwVtxCntZ - dwVtxCntX, dwVtxCntX * dwVtxCntZ - 1, 0, dwVtxCntX - 1);

	SetUp_NeighborCenterIdx(dwVtxCntX, dwVtxCntZ);

	if (NULL == m_pParentNode)
		return E_FAIL;
	return S_OK;
}

void CQuardTree::Decision_In(INDEX32* pIndex, int* pTriCnt)
{
	CFrustum* pFrustum = CFrustum::GetInstance();

	m_pParentNode->Decision_In(pFrustum, pIndex, pTriCnt);
}

void CQuardTree::SetUp_NeighborCenterIdx(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ)
{
	if (NULL == m_pParentNode)
		return;

	m_pParentNode->SetUp_NeighborCenterIdx(dwVtxCntX, dwVtxCntZ);
}

void CQuardTree::Release(void)
{
	::Safe_Release(m_pParentNode);
}

