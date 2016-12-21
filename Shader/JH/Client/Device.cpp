#include "stdafx.h"
#include "Device.h"

IMPLEMENT_SINGLETON(CDevice)

CDevice::CDevice()
{
	m_pDevice = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pDeviceContext = NULL;

	m_pDepthStencilBuffer = NULL;
	m_pDepthStencilView = NULL;

}


CDevice::~CDevice()
{
	Release();
}

HRESULT CDevice::CreateDevice(void)
{

	if (CreateSwapChain() == false)
		return E_FAIL;


	// 장면을 그려 넣고자 하는 후면 버퍼의 부분 직사각형 영영을 뷰포트라 한다
	D3D11_VIEWPORT vp; 
	vp.TopLeftX = 0; // 직사각형의 위치
	vp.TopLeftY = 0; // 직사각형의 위치
	vp.Width = (FLOAT)WINCX; // 직사각형의 크기
	vp.Height = (FLOAT)WINCY; // 직사각형의 크기
	vp.MinDepth = 0.0f; // 최소 깊이 버퍼 값
	vp.MaxDepth = 1.0f; // 최대 깊이 버퍼 값
	
	m_pDeviceContext->RSSetViewports( // Directx에게 뷰포트를 알려줌
		1, // 묶을 뷰포트 개수
		&vp); // 뷰포트 배열을 가리키는 포인터


	return S_OK;
}

bool CDevice::CreateSwapChain(void)
{
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WINCX;
	dxgiSwapChainDesc.BufferDesc.Height = WINCY;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = g_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*0*/;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0, // 우선 d3d 11 지원 여부 점검
		D3D_FEATURE_LEVEL_10_1, // 그다음 d3d 10.1 지원 여부 점검
		D3D_FEATURE_LEVEL_10_0 // 그다음 d3d 10 지원 여부 점검
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pSwapChain, &m_pDevice, &nd3dFeatureLevel, &m_pDeviceContext))) break;
	}

	if (!CreateRenderTargetStanciView()) return(false);


	return true;
}

bool CDevice::CreateRenderTargetStanciView(void)
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;

	if (FAILED(hResult = m_pSwapChain->GetBuffer( // 교환사슬을 가리키는 포인터를 얻음
		0, // 얻고자 하는 후면버퍼의 색인(후면 버퍼를 하나만 쓰기때문에 0)
		__uuidof(ID3D11Texture2D), // 버퍼 인터페이스 형식을 지정
		(LPVOID *)&pd3dBackBuffer))) // 후면 버퍼를 가리키는 포인터를 돌려줌
		return(false);

	if (FAILED(hResult = m_pDevice->CreateRenderTargetView( // 렌더 대상뷰를 생성하는 함수
		pd3dBackBuffer, // 렌더 대상으로 사용할 자원(후면버퍼에 대한 렌더 대상뷰를 생성하므로 방금 얻은 후면버퍼를 지정)
		NULL, // D3D11_RENDER_TARGET_VIEW_DESC 구조체를 가리키는 포인터(형식을 완전히 지정해서 자원을 생성했다면 NULL이여도 댐)
		&m_pRenderTargetView))) // 렌더 대상뷰를 돌려줌
		return(false);

	if (pd3dBackBuffer)
		pd3dBackBuffer->Release(); // GetBuffer을 사용하면 COM참조 횟수가 증가 그래서 다시 릴리즈

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc; // 생성할 텍스처를 서술하는 구조체
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = WINCX; // 텍스처의 너비
	d3dDepthStencilBufferDesc.Height = WINCY; // 텍스처의 높이
	d3dDepthStencilBufferDesc.MipLevels = 1; // 밉맵 수준의 개수(깊이/스텐실 버퍼를 위한 텍스처에서는 밉맵 수준이 하나만 필요)
	d3dDepthStencilBufferDesc.ArraySize = 1; // 텍스처 배열의 텍스처 개수(깊이/스텐실 버퍼의 경우에는 텍스처 하나만 필요)
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 텍셀의 형식을 뜻하는 필드
	// (각 텔셀은[0, 1]구간으로 사상되는 부호없는 24비트 깊이 값 하나와 [0, 255] 구간으로 사상되는 8비트 부호없는 정수 스텐실 값으로 구성)
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 텍스처의 용도를 뜻하는 필드(자원을 GPU가 읽고 써야 한다면 이용도 설정)
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 자원을 파이프라인에 어떤식으로 묶을 것인지를 지정하는 하나 이상의 플래그들을 OR로 결합해서 지정
	// (깊이/스텐실 버퍼의 경우 D3D11_BIND_DEPTH_STENCIL 지정)
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0; // CPU가 자원에 접근하는 방식을 결정하는 플래그들을 지정한다
	// (깊이/스텐실 버퍼의 경우 GPU만 쓰고 CPU는 접근하지 않으므로 0)
	d3dDepthStencilBufferDesc.MiscFlags = 0; // 기타 플래그들로 깊이/스텐실 버퍼에는 적용되지 않으므로 그냥 0

	if (FAILED(hResult = m_pDevice->CreateTexture2D(
		&d3dDepthStencilBufferDesc, // 생성할 텍스처를 서술하는 구조체
		NULL, // 텍스처에 채울 초기 자료를 가리키는 포인터(지금 이텍스처는 깊이/스텐실 버퍼로 사용할 것이므로 따로 자료를 채울필요없다)
		&m_pDepthStencilBuffer))) // 깊이/스텐실 버퍼를 가리키는 포인터를 돌려준다
		return(false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	
	if (FAILED(hResult = m_pDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer, // 뷰를 생성하고자 하는 자원
		&d3dDepthStencilViewDesc/*nullptr*/, // D3D11_DEPTH_STENCIL_VIEW_DESC를 가리키는 포인터
		&m_pDepthStencilView)))  // 깊이/스텐실 뷰를 돌려준다
		return(false);

	m_pDeviceContext->OMSetRenderTargets( // 뷰들을 파이프라인의 출력 병학기 단계에 묶는 함수
		1, // 묶고자 하는 렌더 대상의 개수
		&m_pRenderTargetView, // 파이프라인에 묶을 렌더 대상 뷰들을 가리키는 포인터들을 담은 배열의 첫 원소를 가리키는 포인터
		m_pDepthStencilView); // 파이프라인에 묶을 깊이/스텐실 뷰를 가리키는 포인터


	return true;
}

void CDevice::BeginDevice(void)
{
	D3DXCOLOR DevcieColor(0, 0, 1, 1);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, DevcieColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CDevice::EndDevice(void)
{
	m_pSwapChain->Present(0, 0); // 전면 버퍼와 후면 버퍼를 교환해서 화면에 표시
}

void CDevice::Release(void)
{
	if (m_pDeviceContext)
		m_pDeviceContext->ClearState();

	::Safe_Release(m_pRenderTargetView);
	::Safe_Release(m_pDepthStencilBuffer);
	::Safe_Release(m_pDepthStencilView);
	::Safe_Release(m_pSwapChain);
	::Safe_Release(m_pDeviceContext);
	::Safe_Release(m_pDevice);

}
