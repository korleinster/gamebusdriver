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
	ID3D11Device*					m_pDevice; // 기능 지원 점검과 자원 할당에 쓰이는 인터페이스
	IDXGISwapChain*					m_pSwapChain; // 전면버퍼 텍스처와 후면버퍼 텍스처를 담는 인터페이스
	ID3D11RenderTargetView*			m_pRenderTargetView; // 렌더 대상 뷰
	ID3D11DeviceContext*			m_pDeviceContext; // 렌더 대상을 설정하고, 자원을 그래픽 파이프 라인에 묶고, gpu가 수행할 렌더링 명령들을 지시하는 인터페이스
	ID3D11Texture2D*				m_pDepthStencilBuffer; // 깊이 / 스텐실 버퍼를 가리키는 포인터
	ID3D11DepthStencilView*			m_pDepthStencilView; //깊이/스텐실 뷰

public:
	HRESULT CreateDevice(void);
	bool	CreateSwapChain(void);
	bool	CreateRenderTargetStanciView(void);

public:
	void	BeginDevice(void);
	void	EndDevice(void);

private:
	void Release(void);
};



