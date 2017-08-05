#include "stdafx.h"
#include "HpBar.h"

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
#include "ObjMgr.h"
#include "Font.h"

CHpBar::CHpBar()
{

}

CHpBar::~CHpBar()
{
	CObj::Release();
}

HRESULT CHpBar::Initialize(void)
{
	FAILED_CHECK(AddComponent());

	m_fX = 174.f;
	m_fY = 24.f;
	m_fOriginX = m_fX;
	m_fOriginY = m_fY;
	m_fSizeX = 102.5f;
	m_fSizeY = 10.f;

	m_pFont->m_eType = FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L" ";
	m_pFont->m_fSize = 20.f;
	m_pFont->m_nColor = 0xFF0000FF;
	m_pFont->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(m_fOriginX + 150, m_fOriginY);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	return S_OK;
}

int CHpBar::Update(void)
{
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matView);
	m_matView._11 = m_fSizeX;
	m_matView._22 = m_fSizeY;
	m_matView._33 = 1.f;

	// -0.5	-> -400		,	0.5	-> 400
	m_matView._41 = m_fX - (WINCX >> 1);
	m_matView._42 = -m_fY + (WINCY >> 1);


	UpdateBufferToHp();


	CObj::Update();


	return 0;

}

void CHpBar::Render()
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



CHpBar * CHpBar::Create(void)
{
	CHpBar* pUI = new CHpBar;

	if (FAILED(pUI->Initialize()))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CHpBar::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CRcTex::Create();
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_HpBar");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	m_pFont = CFont::Create(L"Font_Star");

	return S_OK;
}

void CHpBar::UpdateBufferToHp(void)
{
	auto player = CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin();
	float fHp = 1.f - (*player)->GetPacketData()->state.hp / 100.f;

	wchar_t wcHP[MAX_BUF_SIZE];

	wsprintf(wcHP, L"%d / %d", (*player)->GetPacketData()->state.hp, (*player)->GetPacketData()->state.maxhp);

	m_pFont->m_wstrText = wcHP;

	m_fX = m_fOriginX;
	m_fX += fHp * m_fSizeY * 0.25;

	VTXTEX vtx[] =
	{
		{ D3DXVECTOR3(-1.f, 1.f, 0.f),D3DXVECTOR3(0.f, 1.f, 0.f) , D3DXVECTOR2(0.f, 0.f) },
		{ D3DXVECTOR3(1.f - (fHp * 2.f), 1.f, 0.f) , D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f - fHp, 0.f) },
		{ D3DXVECTOR3(1.f - (fHp * 2.f), -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f - fHp, 1.f) },
		{ D3DXVECTOR3(-1.f, -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(0.f, 1.f) }
	};

	CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(this->m_pBuffer->m_VertexBuffer, 0, NULL, vtx, 0, 0);
}

