#pragma once
#include"Include.h"

class CShader;
class CRenderTarget
{
private:
	explicit CRenderTarget();
public:
	virtual ~CRenderTarget();

	void Release();
public:
	HRESULT Ready_RenderTarget(const  UINT&, const  UINT&, DXGI_FORMAT);
	void Ready_DebugBuffer(const float& posX, const float& posY, const float& sizeX, const float& sizeY, const  UINT& type);
	void RenderRT(ID3D11DeviceContext* pd3dImmediateContext);

	ID3D11Texture2D* GetTexture() { return m_pTargetRT; }
	ID3D11ShaderResourceView* GetSRV() { return m_pTagetSRV; }
	ID3D11RenderTargetView* GetRTV() { return m_pTagetRTV; }

private:
	ID3D11Texture2D*			m_pTargetRT;
	ID3D11RenderTargetView*		m_pTagetRTV;
	ID3D11ShaderResourceView*	m_pTagetSRV;

private:
	class CVIBuffer*				m_pDebugBuffer;

	D3DXMATRIX						m_matWorld;

	CShader*						m_pDebugBufferVS;
	CShader*						m_pDebugBufferPS;

	ID3D11SamplerState*				m_pSamplerState;
	ID3D11Buffer*					m_pConstantBuffer;

public:
	static CRenderTarget* Create(const  UINT&, const  UINT&, DXGI_FORMAT);
};