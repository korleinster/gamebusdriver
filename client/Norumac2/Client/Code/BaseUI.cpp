#include "stdafx.h"
#include "BaseUI.h"

#include "Include.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "RcTex.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "ResourcesMgr.h"
#include "Camera.h"
#include "Device.h"
#include "RenderMgr.h"
#include "Font.h"
#include "FontMgr.h"

CBaseUI::CBaseUI()
{

}

CBaseUI::~CBaseUI()
{
	CObj::Release();
}

HRESULT CBaseUI::Initialize(void)
{
	FAILED_CHECK(AddComponent());

	m_fX = 150.f;
	m_fY = 50.f;
	m_fSizeX = 140.f;
	m_fSizeY = 42.f;

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	m_pFont->m_eType = FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L"한글아 나오니?";
	m_pFont->m_fSize = 10.f;
	m_pFont->m_nColor = 0xFF008AFF;
	m_pFont->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(m_fX, m_fY);
	m_pFont->m_fOutlineSize = 7.f;
	m_pFont->m_nOutlineColor = 0xFFFFFFFF;

	return S_OK;
}

int CBaseUI::Update(void)
{
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matView);
	m_matView._11 = m_fSizeX;
	m_matView._22 = m_fSizeY;
	m_matView._33 = 1.f;

	// -0.5	-> -400		,	0.5	-> 400
	m_matView._41 = m_fX - (WINCX >> 1);
	m_matView._42 = -m_fY + (WINCY >> 1);


	CObj::Update();


	return 0;

}

void CBaseUI::Render()
{
	ConstantBuffer cb;

	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &m_matProj);

	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pVertexShader->m_pVertexLayout);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);

	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	m_pBuffer->Render();
	m_pFont->Render();
}

CBaseUI * CBaseUI::Create(void)
{
	CBaseUI* pUI = new CBaseUI;

	if (FAILED(pUI->Initialize()))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CBaseUI::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_BaseUI");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	m_pFont = CFontMgr::GetInstance()->CloneFont(L"Font_Star");

	return S_OK;
}

