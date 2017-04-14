#pragma once
#include "Scene.h"
#include "MouseCol.h"


class CObj;
class CTerrain;
class CMapToolView;
class CComponent;
class CNaviCell;
class CBack :
	public CScene
{
public:
	CMapToolView* m_pMainView;

public:
	CMouseCol*	 m_pMouseCol;
	VTXTEX*		 m_pTerrainVtx;
	CTerrain*	 m_pTerrain;
	map<wstring, CComponent*> m_mapComponent;
	list<CObj*> m_ToolObjList;
	bool		m_bPickFirst;
	bool		m_bPickSecond;
	D3DXVECTOR3	m_vPoint[3];
	CNaviCell*	m_pSelectedCell;
	int			m_iNaviIndex;
	bool		m_bGetMousePt;
	D3DXVECTOR3		m_vGetMousePt;
public:
	CBack();
	virtual ~CBack();

	// CScene을(를) 통해 상속됨
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CBack* Create(void);
	void SetMainView(CMapToolView* pMainView);
	void ConstObjectMode();
	void NaviMeshMode();
	void BuildNavimesh();
	void ComparePoint(D3DXVECTOR3& vPoint1, D3DXVECTOR3& vPoint2, D3DXVECTOR3& vPoint3);
	

public:
	HRESULT	CreateObj(void);
};

