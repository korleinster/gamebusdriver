#include "stdafx.h"
#include "Logo.h"
#include "RcTex.h"
#include "Texture.h"
#include "Shader.h"

#include "ShaderMgr.h"
#include "SceneMgr.h"
#include "Device.h"

CLogo::CLogo()
	:m_pPolygon(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pTexture(NULL)
{
}


CLogo::~CLogo()
{
}

HRESULT CLogo::Initialize(void)
{
	HRESULT hr = NULL;

	hr = AddBuufer();

	if (FAILED(hr))
		return E_FAIL;
	
	return S_OK;
}

int CLogo::Update(void)
{
	if (GetAsyncKeyState(VK_RETURN))
		CSceneMgr::GetInstance()->ChangeScene(SCENE_STAGE);

	return 0;
}

void CLogo::Render(void)
{
	CDevice::GetInstance()->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	CDevice::GetInstance()->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pPolygon->m_ConstantBuffer);
	////////////////
	CDevice::GetInstance()->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	CDevice::GetInstance()->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	// 생성된 샘플러를 해당 셰이더 스테이지에 등록한다
	CDevice::GetInstance()->m_pDeviceContext->PSSetSamplers(
		0, // 샘플러를 설정할 처음 슬롯번호(0~15)
		1, // 설정할 샘플러의 수
		&m_pTexture->m_pSamplerLinear); // 설정할 샘플러 주소

	m_pPolygon->Render();
}

void CLogo::Release(void)
{
	::Safe_Delete(m_pPolygon);

}

CLogo * CLogo::Create(void)
{
	CLogo* pLogo = new CLogo;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);

	}
	return pLogo;
}

HRESULT CLogo::AddBuufer(void)
{
	m_pPolygon = CRcTex::Create();
	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_Logo");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_pTexture = CTexture::Create(L"../Resource/Logo.jpg");

	return S_OK;
}
