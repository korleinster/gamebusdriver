#include "stdafx.h"
#include "Light.h"
#include "Device.h"

CLight::CLight()
{

}

CLight::~CLight()
{

}


HRESULT CLight::Init_Light(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx)
{
	HRESULT hr;

	memcpy(&m_LightInfo, pLightInfo, sizeof(LIGHTINFO));

	// Create constant buffers
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_DIRECTIONAL);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pDirLightCB);
	if (FAILED(hr))
		return hr;

	m_pDirLightCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Directional Light CB"), "Directional Light CB");


	return S_OK;
}

void CLight::Render_Light()
{

}

CLight*	CLight::Create(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx)
{
	CLight* pLight = new CLight;

	if (FAILED(pLight->Init_Light(pLightInfo, dwLightIdx)))
	{
		Safe_Delete(pLight);
	}

	return pLight;
}