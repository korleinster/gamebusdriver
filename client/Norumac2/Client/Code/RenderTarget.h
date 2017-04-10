#pragma once
#include"Include.h"

class CRenderTarget
{
private:
	explicit CRenderTarget();
public:
	virtual ~CRenderTarget();
public:
	HRESULT Ready_RenderTarget(const  UINT&, const  UINT&, DXGI_FORMAT);
private:


	ID3D11Texture2D*			m_pTargetRT;
	
	ID3D11RenderTargetView*		m_pTagetRTV;
	
	ID3D11ShaderResourceView*	m_pTagetSRV;

public:
	static CRenderTarget* Create(const  UINT&, const  UINT&, DXGI_FORMAT);
};