#pragma once
#include"Include.h"

class CDeviceMgr;

class CMainApp
{
private:
	CDeviceMgr*	m_pDevice;
public:
	HRESULT		Init_MainApp();
	void		Update_MainApp();
	void		Render_MainApp();
	void		Release_MainApp();
public:
	static CMainApp* Create(void);
public:
	CMainApp();
	~CMainApp();
};