#pragma once

#include "Include.h"


const DWORD MAX = -1;

class CFrustum;
class CNode
{
public:
	enum NODE { NODE_LT, NODE_RT, NODE_LB, NODE_RB, NODE_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_LB, CORNER_RB, CORNER_END };
	enum NEIGHBORCENTER { CENTER_LEFT, CENTER_TOP, CENTER_RIGHT, CENTER_BOTTOM, CENTER_END };

public:
	CNode(void);
	~CNode();
public:
	void Init_Node(const D3DXVECTOR3* pVtxPos, const DWORD& dwLT, const DWORD& dwRT, const DWORD& dwLB, const DWORD& dwRB);
	void Decision_In(CFrustum* pFrustum, INDEX32* pIndex, int* pTriCnt);
	void SetUp_NeighborCenterIdx(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ);

public:
	static CNode* Create(const D3DXVECTOR3* pVtxPos, const DWORD& dwLT, const DWORD& dwRT, const DWORD& dwLB, const DWORD& dwRB);
public:
	CNode*			m_pChildNode[NODE_END];
	DWORD			m_dwCorner[CORNER_END];
	DWORD			m_dwCenter;
	float			m_fRadius;
	const D3DXVECTOR3*	m_pVtxPos;
	DWORD			m_dwNeighborCenter[CENTER_END];

public:
	bool LevelofDetail(void);
	bool LevelofDetail(const DWORD& dwCenter);
	void Compute_NeighborCenter(const DWORD& dwVtxCntX, const DWORD& dwVtxCntZ);
	void Release(void);

};