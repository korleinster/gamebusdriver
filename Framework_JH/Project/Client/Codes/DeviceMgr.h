#pragma once

#include "Include.h"

class CDeviceMgr
{
	DECLARE_SINGLETON(CDeviceMgr)
public:
	enum WinMode { WIN_MODE_FULL, WIN_MODE_WIN };
private:
	static const float COLOR_BACKBUFFER[4];
private:
	ID3D11Device*			m_pDevice; // 컴퓨터 하드웨어의 기능 지원 점검, Resource 할당
	ID3D11DeviceContext*	m_pDeviceContext; // 렌더링 대상 설정, Resource를 그래픽 파이프라인에 바인딩, GPU가 수행할 렌더링 명령 지시
	IDXGISwapChain*			m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D*		m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11BlendState*		m_pBlendState;
public:
	HRESULT		Init(WinMode _eWinMode = WIN_MODE_WIN);
	void		Release();
	void		Render_Begin();
	void		Render_End();
	void		Blend_Begin();
	void		Blend_End();
private:
	HRESULT		Init_Blend();
public:
	ID3D11Device*			GetDevice() { return m_pDevice; }
	ID3D11DeviceContext*	GetDeviceContext() { return m_pDeviceContext; }
	IDXGISwapChain*			GetSwapChain() { return m_pSwapChain; }
	ID3D11RenderTargetView*	GetRenderTargetView() { return m_pRenderTargetView; }
	ID3D11Texture2D*		GetSDepthStencilBuffer() { return m_pDepthStencilBuffer; }
	ID3D11DepthStencilView*	GetDepthStencilView() { return m_pDepthStencilView; }
	ID3D11BlendState*		GetBlendState() { return m_pBlendState; }
private:
	explicit CDeviceMgr();
	~CDeviceMgr();
};