#include "stdafx.h"
#include "ChatUI.h"

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
#include <conio.h>
#include "SceneMgr.h"

CChatUI::CChatUI()
{

}

CChatUI::~CChatUI()
{
	CObj::Release();
}

HRESULT CChatUI::Initialize(void)
{
	FAILED_CHECK(AddComponent());

	ZeroMemory(&m_cChat, sizeof(char) * MAX_BUF_SIZE);

	m_fX = 150.f;
	m_fY = 550.f;
	m_fSizeX = 135.5f;
	m_fSizeY = 86.f;

	m_pFont->m_eType = FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L"";
	m_pFont->m_fSize = 20.f;
	m_pFont->m_nColor = 0xFFFFFFFF;
	m_pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(15.f, 640.f);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;



	m_pStateFont->m_eType = FONT_TYPE_OUTLINE;
	m_pStateFont->m_wstrText = L"채팅Off";
	m_pStateFont->m_fSize = 20.f;
	m_pStateFont->m_nColor = 0xFFFFFFFF;
	m_pStateFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pStateFont->m_vPos = D3DXVECTOR2(15.f, 660.f);
	m_pStateFont->m_fOutlineSize = 1.f;
	m_pStateFont->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);
	return S_OK;
}

int CChatUI::Update(void)
{
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matView);
	m_matView._11 = m_fSizeX;
	m_matView._22 = m_fSizeY;
	m_matView._33 = 1.f;

	// -0.5	-> -400		,	0.5	-> 400
	m_matView._41 = m_fX - (WINCX >> 1);
	m_matView._42 = -m_fY + (WINCY >> 1);


	if (g_bChatMode == true)
	{
		m_pStateFont->m_wstrText = L"채팅On";
	}
	else
	{
		m_pStateFont->m_wstrText = L"채팅Off";
	}

	CObj::Update();


	return 0;

}

void CChatUI::Render()
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

	if (CSceneMgr::GetInstance()->GetScene() == SCENE_STAGE)
		m_pStateFont->Render();
	
	for (auto FontList : m_ChatLogList)
		FontList->Render();
}

CChatUI * CChatUI::Create(void)
{
	CChatUI* pUI = new CChatUI;

	if (FAILED(pUI->Initialize()))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CChatUI::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_ChatUI");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	m_pFont = CFont::Create(L"../Resource/Font/Ko_Star.ttf", L"Ko_Star");
	m_pStateFont = CFont::Create(L"../Resource/Font/Ko_Star.ttf", L"Ko_Star");

	return S_OK;
}

