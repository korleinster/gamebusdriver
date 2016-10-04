#pragma once
#include "Include.h"

class CMainApp
{
public:
	HRESULT		InitApp();
	void		Update();
	void		Render();
	void		Release();
public:
	static CMainApp* Create(void);
public:
	CMainApp();
	~CMainApp();
};