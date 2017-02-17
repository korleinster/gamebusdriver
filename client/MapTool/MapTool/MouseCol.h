#pragma once

#include "Include.h"
#include "MapToolView.h"


class CMouseCol
{
private:
	int		m_iCntX;
	int		m_iCntZ;
	int		m_iItv;
public:
	CMouseCol(void);
	~CMouseCol(void);

private:
	D3DXVECTOR3					m_vPivotPos;
	D3DXVECTOR3					m_vRayDir;
	D3DXVECTOR3					m_CurMPos;

private:
	void	Translation_ViewSpace(void);
	void	Translation_Local(const D3DXMATRIX*	pmatWolrd);
	bool	Translation_ViewSpace2(void);

public:
	static POINT	GetMousePos(void);
	void PickTerrain(D3DXVECTOR3* pOut, const VTXTEX* pTerrainVtx);
	//void PickTerrainVertex(D3DXVECTOR3* pOut, const VTXTEX* pTerrainVtx);
	//bool PickObject(D3DXVECTOR3* pOut, const VTXTEX* pVertex, const D3DXMATRIX* pmatWorld);
	//bool PickObject(D3DXVECTOR3* pOut, VTXCOL* pVertex, INDEX16* pIndex, const D3DXMATRIX* pmatWorld);

	//bool PickTerrainIndex(D3DXVECTOR3* pOut, const VTXTEX* pTerrainVtx);
	//bool PickObject(D3DXVECTOR3* pOut, CToolsView::MAPCUBE* pMapCube, size_t iMode);
	void SetSize(int iCntX, int iCntZ, int iItv);
	bool MeshPick(D3DXVECTOR3* pOut, vector<VTXTEX>* pVecVtx, int iCnt);
	//bool PickNaviCell(D3DXVECTOR3* pOut, CNaviCell* pCell, int* pOutIndex);
	//bool PickOccBox(D3DXVECTOR3* pOut, VTXCOL* pVertex, INDEX16* pIndex, const D3DXMATRIX* pmatWorld);


public:
	HRESULT	Initialize(void);

private:
	void Release(void);
};
