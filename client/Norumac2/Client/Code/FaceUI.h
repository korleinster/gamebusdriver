#pragma once
#include "UI.h"

class CFaceUI : public CUI
{
public:
	CFaceUI();
	virtual ~CFaceUI();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CFaceUI* Create();

	
};