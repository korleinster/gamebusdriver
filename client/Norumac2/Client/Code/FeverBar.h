#pragma once
#include "UI.h"

class CFeverBar : public CUI
{
public:
	CFeverBar();
	virtual ~CFeverBar();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CFeverBar* Create();


};