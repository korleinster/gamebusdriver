#pragma once
#include "Include.h"

class CRenderTarget;
class CShader;
class CVIBuffer;
class CMultiRenderTarget
{
public:
	CMultiRenderTarget();
	virtual ~CMultiRenderTarget();

	HRESULT Initialize(UINT width, UINT height);
	void Release();

	void Begin_MRT(ID3D11DeviceContext* pd3dImmediateContext);
	void End_MRT(ID3D11DeviceContext* pd3dImmediateContext);
	void PrepareForUnpack(ID3D11DeviceContext* pd3dImmediateContext);

	ID3D11DepthStencilView* GetDepthDSV() { return m_DepthStencilDSV; }
	ID3D11DepthStencilView* GetDepthReadOnlyDSV() { return m_DepthStencilReadOnlyDSV; }

	ID3D11ShaderResourceView* GetDepthView() { return m_DepthStencilSRV; }

	CRenderTarget* GetRT(int index)
	{
		if (index >= m_vecRT.size())
		{
			return NULL;
		}

		return m_vecRT[index];
	}

	void RenderMRT(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pSRV = NULL);
	void RenderDepth(ID3D11DeviceContext* pd3dImmediateContext);

public:
	void SetRT(CRenderTarget* pRT);

private:
	ID3D11Buffer*			m_pGBufferUnpackCB;
	// Depth textures
	ID3D11Texture2D*		m_DepthStencilRT;

	// Depth render views
	ID3D11DepthStencilView* m_DepthStencilDSV;
	ID3D11DepthStencilView* m_DepthStencilReadOnlyDSV;

	// Depth shader resource views
	ID3D11ShaderResourceView* m_DepthStencilSRV;
	ID3D11DepthStencilState* m_DepthStencilState;


	vector<CRenderTarget*>	m_vecRT;

	CShader*				m_pGBufferVisVertexShader;
	CShader*				m_pGBufferVisPixelShader;

	CVIBuffer*				m_pDepthDebugBuffer;
	ID3D11Buffer*			m_pCB;
	ID3D11SamplerState*		m_pSamplerState;
	D3DXMATRIX				m_matDepthWorld;
};