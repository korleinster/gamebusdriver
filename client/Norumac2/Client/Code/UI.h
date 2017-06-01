#pragma once
#include "Obj.h"


class CShader;
class CVIBuffer;
class CTexture;
class CUI :public CObj
{
public:
	CUI();
	virtual ~CUI();

public:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	
public:
	float			m_fX, m_fY;
	float			m_fSizeX, m_fSizeY;
	float			m_fOriginX, m_fOriginY;
	D3DXMATRIX		m_matView, m_matProj;
	
public:
	virtual HRESULT Initialize(void) {return S_OK;};
	virtual int		Update(void) {return 0;};
	virtual void	Render(void) { return; };

public:
	static CUI* Create(void) { return NULL; }
	HRESULT AddComponent(void) { return S_OK; }
};