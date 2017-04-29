#pragma once
#include "Include.h"

class CShader;
class CMultiRenderTarget;
class CTargetMgr
{
public:
	DECLARE_SINGLETON(CTargetMgr)

	CTargetMgr();
	virtual ~CTargetMgr();

public:
	HRESULT Initialize();
	void Release();

	CMultiRenderTarget* GetGBuffer() { return m_pMRT_GBuffer; }
	CMultiRenderTarget* GetBorderMRT() { return m_pMRT_Border; }
	void RenderGBuffer(ID3D11DeviceContext* pd3dImmediateContext);

private:
	CMultiRenderTarget*		m_pMRT_GBuffer;
	CMultiRenderTarget*		m_pMRT_Border;

	//CShader*				m_pGBufferVisVertexShader;
	//CShader*				m_pGBufferVisPixelShader;
	//ID3D11VertexShader*		m_pGBufferVisVertexShader;
	//ID3D11PixelShader*		m_pGBufferVisPixelShader;
};

