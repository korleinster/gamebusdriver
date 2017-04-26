#pragma once
#include "UI.h"

class CFeverZero : public CUI
{
public:
	CFeverZero();
	virtual ~CFeverZero();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CFeverZero* Create();


};