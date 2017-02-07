#include "stdafx.h"
#include "Texture.h"
#include "Device.h"


CTexture::CTexture()
{
	m_pTextureRV = NULL;
	m_pSamplerLinear = NULL;
	m_pBlendState = NULL;
}

CTexture::CTexture(const CTexture & rhs)
{
	m_pTextureRV = rhs.m_pTextureRV;
	m_pSamplerLinear = rhs.m_pSamplerLinear;
	m_pBlendState = rhs.m_pBlendState;
	m_dwRefCount = rhs.m_dwRefCount;
	++m_dwRefCount;
}

CTexture::~CTexture()
{
	Release();
}

HRESULT CTexture::CreateTexture(LPCWSTR szFileFath)
{
	CDevice* pGrapicDev = CDevice::GetInstance();
	HRESULT hr = NULL;
	hr = D3DX11CreateShaderResourceViewFromFile(pGrapicDev->m_pDevice, szFileFath, NULL, NULL, &m_pTextureRV, NULL);
	if (FAILED(hr))
		return E_FAIL;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pGrapicDev->m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

CTexture * CTexture::Create(LPCWSTR szFileFath)
{
	CTexture* pTexture = new CTexture;

	if (FAILED(pTexture->CreateTexture(szFileFath)))
	{
		::Safe_Delete(pTexture);
	}

	return pTexture;
}

CResources * CTexture::CloneResource(void)
{
	return new CTexture(*this);
}

DWORD CTexture::Release(void)
{
	if (m_dwRefCount == 1)
	{
		::Safe_Release(m_pTextureRV);
		::Safe_Release(m_pSamplerLinear);
		::Safe_Release(m_pBlendState);
	}
	else
		--m_dwRefCount;

	return 0;
}
