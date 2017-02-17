#pragma once
#include "Scene.h"
#include "MouseCol.h"


class CObj;
class CTerrain;
class CMapToolView;
class CComponent;
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

public:
	HRESULT	CreateObj(void);
};

