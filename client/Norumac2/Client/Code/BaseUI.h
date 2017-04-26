#pragma once
#include "UI.h"

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
	static CBaseUI* Create();


};