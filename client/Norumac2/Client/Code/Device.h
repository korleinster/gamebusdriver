#pragma once
#include "include.h"

class CTimeMgr;
class CDevice
{
public:
	CDevice();
	virtual ~CDevice();

public:
	DECLARE_SINGLETON(CDevice)

public:
	ID3D11Device*					m_pDevice;
	IDXGISwapChain*					m_pSwapChain;
	ID3D11RenderTargetView*			m_pRenderTargetView;
	ID3D11DeviceContext*			m_pDeviceContext;

	ID3D11Texture2D*				m_pDepthStencilBuffer;
	ID3D11DepthStencilView*			m_pDepthStencilView;
	ID3D11BlendState*				m_pBlendState;
public:
	HRESULT CreateDevice(void);
	bool	CreateSwapChain(void);
	bool	CreateRenderTargetStanciView(void);
	ID3D11BlendState*		GetBlendState() { return m_pBlendState; }

	void	Blend_Begin();
	void	Blend_End();
	void	BeginDevice(void);
	void	EndDevice(void);
	void	BeginDeferredContext(void);
	void	EndDeferredContext(ID3D11CommandList* pCommandList);
	void Release(void);
private:
	HRESULT		Init_Blend();
};



