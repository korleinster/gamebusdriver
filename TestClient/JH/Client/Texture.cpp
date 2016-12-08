#include "stdafx.h"
#include "Texture.h"
#include "Device.h"
//#include "DirectXTex.h"

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

HRESULT CTexture::CreateTexture(LPCWSTR szFilePath)
{
	CDevice* pGrapicDev = CDevice::GetInstance();
	HRESULT hr = NULL;
	hr = D3DX11CreateShaderResourceViewFromFile(pGrapicDev->m_pDevice, szFilePath, NULL, NULL, &m_pTextureRV, NULL);
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

HRESULT CTexture::CreateCubeTexture(LPCWSTR szFilePath)
{
	HRESULT hr = NULL;

	CDevice* pGrapicDev = CDevice::GetInstance();

	D3DX11_IMAGE_LOAD_INFO tLoadInfo;
	tLoadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	ID3D11Texture2D* pTexture2D = NULL;

	hr = D3DX11CreateTextureFromFile(
		pGrapicDev->m_pDevice, szFilePath,
		&tLoadInfo, NULL, (ID3D11Resource**)&pTexture2D, NULL);

	if (FAILED(hr))
		return E_FAIL;

	D3D11_TEXTURE2D_DESC tTextureData;
	pTexture2D->GetDesc(&tTextureData);

	D3D11_SHADER_RESOURCE_VIEW_DESC tViewData;
	tViewData.Format = tTextureData.Format;
	tViewData.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	tViewData.TextureCube.MipLevels = tTextureData.MipLevels;
	tViewData.TextureCube.MostDetailedMip = 0;

	pGrapicDev->m_pDevice->
		CreateShaderResourceView(pTexture2D, &tViewData, &m_pTextureRV);

	ID3D11SamplerState* pSamplerState = NULL;
	D3D11_SAMPLER_DESC tSamplerData;

	ZeroMemory(&tSamplerData, sizeof(D3D11_SAMPLER_DESC));
	tSamplerData.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSamplerData.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerData.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerData.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerData.ComparisonFunc = D3D11_COMPARISON_NEVER;
	tSamplerData.MinLOD = 0;
	tSamplerData.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pGrapicDev->m_pDevice->CreateSamplerState(&tSamplerData, &m_pSamplerLinear);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CTexture::Render()
{
	CDevice::GetInstance()->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	CDevice::GetInstance()->m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
}

CTexture * CTexture::Create(LPCWSTR szFilePath)
{
	CTexture* pTexture = new CTexture;

	if (FAILED(pTexture->CreateTexture(szFilePath)))
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
	return 0;
}