#pragma once
#include "UI.h"

class CRuneBar : public CUI
{
public:
	CRuneBar();
	virtual ~CRuneBar();
	//

public:
	bool m_bRender;
public:
	virtual HRESULT Initialize(float fX, float fY);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CRuneBar* Create(float fX, float fY);


};