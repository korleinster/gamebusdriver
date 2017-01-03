#pragma once

#include "Include.h"

class CLight;
class CLightMgr
{
public:
	DECLARE_SINGLETON(CLightMgr)
private:
	CLightMgr(void);
	~CLightMgr(void);
public:
	const LIGHTINFO* Get_LightInfo(const DWORD& dwIndex);
public:
	HRESULT AddLight(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx);
	void Render_Light();
private:
	typedef list<CLight*>	LIGHTLIST;
	LIGHTLIST				m_LightList;
public:
	void Release(void);
};