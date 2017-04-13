#include "stdafx.h"
#include "LogoBack.h"
#include "RcTex.h"
#include "Texture.h"
#include "Shader.h"

#include "ShaderMgr.h"
#include "SceneMgr.h"
#include "Device.h"
#include "ResourcesMgr.h"
#include "Info.h"
#include "RenderMgr.h"

CLogoBack::CLogoBack()
	:m_pPolygon(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pTexture(NULL),
	m_pInfo(NULL)
{

}


CLogoBack::~CLogoBack()
{
	CObj::Release();
}

HRESULT CLogoBack::Initialize(void)
{
	HRESULT hr = NULL;

	hr = AddBuffer();

	if (FAILED(hr))
		return E_FAIL;

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	return S_OK;
}

int CLogoBack::Update(void)
{
	return 0;
}

void CLogoBack::Render(void)
{
	CDevice::GetInstance()->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	CDevice::GetInstance()->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pPolygon->m_ConstantBuffer);
	//////////////////
	CDevice::GetInstance()->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	CDevice::GetInstance()->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	CDevice::GetInstance()->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	m_pPolygon->Render();
}

CLogoBack * CLogoBack::Create(void)
{
	CLogoBack* pLogoBack = new CLogoBack;
	if (FAILED(pLogoBack->Initialize()))
	{
		::Safe_Delete(pLogoBack);

	}
	return pLogoBack;
}

HRESULT CLogoBack::AddBuffer(void)
{

	CComponent*		pComponent = NULL;

	//Transform
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex");
	m_pPolygon = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_LOGO, L"Texture_Logo");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	//m_pPolygon = CRcTex::Create();
	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_Logo");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));
	//m_pTexture = CTexture::Create(L"../Resource/Logo.jpg");

	return S_OK;
}
