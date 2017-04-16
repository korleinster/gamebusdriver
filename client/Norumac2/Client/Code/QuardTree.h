#pragma once
#include "Include.h"

class CQuardTree
{
public:
	DECLARE_SINGLETON(CQuardTree)

public:
	CQuardTree(void);
	~CQuardTree(void);

public:
	HRESULT Initialize_QuadTree(const D3DXVECTOR3* pVtxPos, const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ);
	void Decision_In(INDEX32* pIndex, int* pTriCnt);
private:
	CNode*			m_pParentNode;
private:
	void SetUp_NeighborCenterIdx(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ);
public:
	void Release(void);
};