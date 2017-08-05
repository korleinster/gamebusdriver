#pragma once
#include "UI.h"


class CFont;
class CBottomUI : public CUI
{
public:
	CBottomUI();
	virtual ~CBottomUI();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:

public:
	static CBottomUI* Create();


};