#pragma once
#include"Include.h"

class CRenderTarget
{
private:
	explicit CRenderTarget();
public:
	virtual ~CRenderTarget();
public:
	HRESULT Ready_RenderTarget(const  UINT&, const  UINT&, DXGI_FORMAT,D3DXCOLOR);
	HRESULT Ready_DebugBuffer(const float&, const float&, const float&, const float&);

	void Release(void);

	ID3D11Texture2D*			GetTargetTexture() { return m_pTargetRT; }
	ID3D11ShaderResourceView*	GetTargetSRV() { return m_pTargetSRV; }
private:

	ID3D11Buffer*				m_pGBufferUnpackCB;

	ID3D11Texture2D*			m_pTargetRT;
	
	ID3D11RenderTargetView*		m_pTargetRTV;
	
	ID3D11ShaderResourceView*	m_pTargetSRV;

	D3DXCOLOR					m_ClearColor;
public:
	static CRenderTarget* Create(const  UINT&, const  UINT&, DXGI_FORMAT, D3DXCOLOR);
};