#pragma once
#include "UI.h"

class CHpBar : public CUI
{
public:
	CHpBar();
	virtual ~CHpBar();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	void UpdateBufferToHp(void);

public:
	virtual HRESULT AddComponent(void);

public:
	static CHpBar* Create();


};