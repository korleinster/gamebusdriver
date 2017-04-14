#pragma once

#include "Include.h"


class CLineCol;
class CLine2D;
class CShader;
class CVIBuffer;
class CInfo;
class CNaviCell
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
public:
	 CNaviCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);
	 ~CNaviCell(void);

public:
	D3DXVECTOR3* GetPoint(POINT ePointID);
	CNaviCell* GetNeighbor(NEIGHBOR eNeighbor);
	DWORD GetIndex(void);
	DWORD GetType(void);
	void					SetIndex(DWORD dwIndex);
	CLine2D*	GetLine(LINE eLineID);
	void SetPoint(D3DXVECTOR3 vDest, POINT ePointID);


public:
	void SetNeighbor(NEIGHBOR eNeighborID, CNaviCell* pNeighbor);

public:
	HRESULT Init_Cell(const DWORD& dwIdx, const DWORD& dwType);
	bool ComparePoint(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, CNaviCell* pNeighbor);
	bool CheckPass(const D3DXVECTOR3* pPos
		, const D3DXVECTOR3* pDir
		, NEIGHBOR* peNeighborID);
	void Render(void);
	D3DXVECTOR3 SlidingVector(D3DXVECTOR3& vDir, DWORD dwNeighborID);
	bool CheckPass(const D3DXVECTOR3* pPos
		, const D3DXVECTOR3* pDir
		, DWORD dwNeighborID);
	HRESULT AddComponent(CLineCol* pLineCol);
	void Update();
	void Release();

private:
	CLine2D*				m_pLine2D[LINE_END];
	CNaviCell*				m_pNeighbor[NEIGHBOR_END];
	CShader*				m_pVertexShader;
	CShader*				m_pPixelShader;
	CVIBuffer*				m_pBuffer;
public:
	D3DXVECTOR3				m_CellPos;
	D3DXMATRIX				m_CellMat;

private:
	D3DXVECTOR3			m_vPoint[POINT_END];
	//LPD3DXLINE			m_pLine;
	DWORD				m_dwIndex;
	DWORD				m_dwType;

public:
	static CNaviCell* Create(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC
		, const DWORD& dwIdx, const DWORD& dwType);


};