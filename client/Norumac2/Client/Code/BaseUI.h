#pragma once
#include "UI.h"


class CFont;
class CBaseUI : public CUI
{
public:
	CBaseUI();
	virtual ~CBaseUI();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	CFont* m_pFont;

public:
	static CBaseUI* Create();


};