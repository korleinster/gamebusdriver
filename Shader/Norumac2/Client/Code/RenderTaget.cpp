#include"stdafx.h"
#include"RenderTarget.h"
#include "Device.h"

#pragma pack(push,1)
struct CB_GBUFFER_UNPACK
{
	D3DXVECTOR4 PerspectiveValues;
	D3DXMATRIX  ViewInv;
};
#pragma pack(pop)

CRenderTarget::CRenderTarget()
	: m_pGBufferUnpackCB(nullptr),
	m_pTargetRT(nullptr),
	m_pTargetRTV(nullptr),
	m_pTargetSRV(nullptr)
{
	m_ClearColor = { 0.f, 0.f, 0.f, 0.f };
}

CRenderTarget::~CRenderTarget()
{

}

HRESULT CRenderTarget::Ready_RenderTarget(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format, D3DXCOLOR color)
{
	Release();

	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC textureDesc = {
		sizeX, //UINT Width;
		sizeY, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		format, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags; D3D11_BIND_RENDER_TARGET | 
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	hr = CDevice::GetInstance()->m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pTargetRT);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc =
	{
		format,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	hr = CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_pTargetRT, &renderTargetViewDesc, &m_pTargetRTV);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
	{
		format,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0 
	};

	hr = CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_pTargetRT, &shaderResourceViewDesc, &m_pTargetSRV);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.ByteWidth = sizeof(CB_GBUFFER_UNPACK);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&constBufferDesc, NULL, &m_pGBufferUnpackCB);
	if (FAILED(hr))
	{
		return hr;
	}

	m_ClearColor = color;

	return S_OK;
}

HRESULT CRenderTarget::Ready_DebugBuffer(const float& posX, const float& posY, const float& sizeX, const float& sizeY)
{

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format, D3DXCOLOR color)
{
	CRenderTarget*	pInstance = new CRenderTarget();

	if (FAILED(pInstance->Ready_RenderTarget(sizeX, sizeY, format, color)))
	{
		MessageBox(NULL,  L"System Message", L"RenderTarget Created Failed", MB_OK);
		Safe_Delete(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Release()
{
	Safe_Release(m_pGBufferUnpackCB);
	Safe_Release(m_pTargetRT);
	Safe_Release(m_pTargetRTV);
	Safe_Release(m_pTargetSRV);
}