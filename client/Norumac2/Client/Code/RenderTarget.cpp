#include"stdafx.h"
#include"RenderTarget.h"
#include "Device.h"

CRenderTarget::CRenderTarget()
{

}

CRenderTarget::~CRenderTarget()
{

}

HRESULT CRenderTarget::Ready_RenderTarget(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format)
{
	// Allocate the depth stencil target
	D3D11_TEXTURE2D_DESC dtd = {
		sizeX, //UINT Width;
		sizeY, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		format, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, nullptr, &m_pTargetRT);

	// Create the render target views

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		format,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_pTargetRT, &rtsvd, &m_pTagetRTV);

	// Create the resource views
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd =
	{
		format,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};

	CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_pTargetRT, &dsrvd, &m_pTagetSRV);

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format)
{
	CRenderTarget*	pInstance = new CRenderTarget();

	if (FAILED(pInstance->Ready_RenderTarget(sizeX, sizeY, format)))
	{
		MessageBox(NULL,  L"System Message", L"RenderTarget Created Failed", MB_OK);
		Safe_Delete(pInstance);
	}

	return pInstance;
}