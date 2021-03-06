#pragma once
#include "Scene.h"
class CStage :
	public CScene
{
public:
	CStage();
	virtual ~CStage();

	// CScene을(를) 통해 상속됨
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CStage* Create(void);
	
public:
	bool m_bNaviRender;
	bool m_bBGMStart;

public:
	HRESULT	CreateObj(void);
	void DataLoad(void);
	void NaviLoad(void);

public:
	bool m_bFirstLogin;
};

