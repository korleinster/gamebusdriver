#pragma once
#include "Include.h"


class CLineCol;
class CNaviCell;
class CNaviMgr
{
public:
	DECLARE_SINGLETON(CNaviMgr)

public:
	CNaviMgr(void);
	~CNaviMgr(void);

public:
	HRESULT Reserve_CellContainerSize(const DWORD& dwSize);
	HRESULT Add_Cell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, const DWORD& dwType);
	void Link_Cell(void);
	void Unlink_Cell(void);
	void Render(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir
		, const DWORD& dwCurrentIdx);
	void Update(void);
	void Release(void);

public:
	int GetSize(void);
	void GetNearPoint(D3DXVECTOR3& vPoint, float fRange);
	bool SetOnePoint(D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2, D3DXVECTOR3 vPoint3);
	bool SetTwoPoint(D3DXVECTOR2 vDir, CNaviCell* pNaviClose, D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2);
	void ChangePoint(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour);
	void DeletePoint(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour);
	void DeleteTriangle(D3DXVECTOR3 vDest, D3DXVECTOR3 vSour);
	void ResetNavimesh(void);
	void ResetIndex(void);

private:
	DWORD		m_dwReserveSize;

	typedef vector<CNaviCell*>		VECCELL;
	typedef vector<CLineCol*>		VECLINE;
	VECCELL		m_vecNaviMesh;
	VECLINE		m_vecNaviLine;

public:
	vector<CNaviCell*>*	GetCell(void)
	{
		return &m_vecNaviMesh;
	}

private:
	DWORD		m_dwIdxCnt;

};