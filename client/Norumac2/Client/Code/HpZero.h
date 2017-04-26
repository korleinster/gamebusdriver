#pragma once
#include "UI.h"

class CHpZero : public CUI
{
public:
	CHpZero();
	virtual ~CHpZero();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CHpZero* Create();


};