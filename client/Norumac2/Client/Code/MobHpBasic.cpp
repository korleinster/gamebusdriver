#include "stdafx.h"
#include "MobHpBasic.h"

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
#include "TimeMgr.h"

CMobHpBasic::CMobHpBasic()
{

}

CMobHpBasic::~CMobHpBasic()
{
	CObj::Release();
	CRenderMgr::GetInstance()->DelRenderGroup(TYPE_UI, this);//임시인데 어디다 빼야할까
}

HRESULT CMobHpBasic::Initialize(void)
{
	FAILED_CHECK(AddComponent());

	m_fX = 150.f;
	m_fY = 150.f;
	m_fSizeX = 115.f;
	m_fSizeY = 5.f;
	m_fRendTime = 0.f;

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	m_pFont->m_eType = FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L" ";
	m_pFont->m_fSize = 20.f;
	m_pFont->m_nColor = 0xFF008AFF;
	m_pFont->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(m_fX, m_fY - 15.f);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;

	return S_OK;
}

int CMobHpBasic::Update(void)
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


	m_fRendTime += CTimeMgr::GetInstance()->GetTime();

	if (m_fRendTime > 5.f)
		m_bDeath = true;

	if (!m_bDeath)
		return 0;

	else
		return 100;

	return 0;

}

void CMobHpBasic::Render()
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

void CMobHpBasic::SetName(wstring wstrText)
{
	m_pFont->m_wstrText = wstrText;
}

CMobHpBasic * CMobHpBasic::Create(void)
{
	CMobHpBasic* pUI = new CMobHpBasic;

	if (FAILED(pUI->Initialize()))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CMobHpBasic::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_MobBasicHp");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	m_pFont = CFont::Create(L"../Resource/Font/Ko_Star.ttf", L"Ko_Star");

	return S_OK;
}

