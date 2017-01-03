#pragma once

#include "Include.h"

class CLight
{
public:
	explicit CLight();
	~CLight(void);
public:
	const LIGHTINFO* Get_LightInfo(void) const
	{
		return &m_LightInfo;
	}
public:
	HRESULT Init_Light(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx);
	void Render_Light();
	void Release();
public:
	static CLight* Create(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx);
private:
	LPDIRECT3DDEVICE9				m_pDevice;
	LIGHTINFO						m_LightInfo;
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;
};