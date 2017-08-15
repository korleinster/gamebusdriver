#include "stdafx.h"
#include "SkillUI.h"

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
#include "Player.h"

CSkillUI::CSkillUI()
{

}

CSkillUI::~CSkillUI()
{
	CObj::Release();
}

HRESULT CSkillUI::Initialize(SKILLUITYPE eUiType, wstring wstUiImage, D3DXVECTOR2 vUIPos)
{
	FAILED_CHECK(AddComponent(wstUiImage));

	m_fX = vUIPos.x;
	m_fY = vUIPos.y;
	m_fOriginX = m_fX;
	m_fOriginY = m_fY;
	m_fSizeX = 19.5f;
	m_fSizeY = 19.5f;

	m_eUiType = eUiType;

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	return S_OK;
}

int CSkillUI::Update(void)
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


	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()));

	if(m_eUiType == UI_POTION && (*pPlayer).m_bPotionCool)
		UpdateBufferToCool();

	if (m_eUiType == UI_TPROCK && (*pPlayer).m_bTpCool)
		UpdateBufferToCool();

	if (m_eUiType == UI_SKILL1 && (*pPlayer).m_bSkil1Cool)
		UpdateBufferToCool();

	if (m_eUiType == UI_SKILL2 && (*pPlayer).m_bSkil2Cool)
		UpdateBufferToCool();


	return 0;

}

void CSkillUI::Render()
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
}

CSkillUI * CSkillUI::Create(SKILLUITYPE eUiType, wstring wstUiImage, D3DXVECTOR2 vUIPos)
{
	CSkillUI* pUI = new CSkillUI;

	if (FAILED(pUI->Initialize(eUiType, wstUiImage, vUIPos)))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CSkillUI::AddComponent(wstring wstUiImage)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CRcTex::Create(); /*CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI")*/;
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, wstUiImage);
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	return S_OK;
}

void CSkillUI::UpdateBufferToCool(void)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()));
	float fCoolTime;
	
	

	if (m_eUiType == UI_POTION)
	{
		fCoolTime = (1.f - (*pPlayer).m_fPotionTime / 10.f) ;
	}

	if (m_eUiType == UI_TPROCK)
	{
		fCoolTime = 1.f - (*pPlayer).m_fTpTime / 5.f;
	}

	if (m_eUiType == UI_SKILL1)
	{
		fCoolTime = 1.f - (*pPlayer).m_fSkill1Time / 1.f;
	}

	if (m_eUiType == UI_SKILL2)
	{
		fCoolTime = 1.f - (*pPlayer).m_fSkill2Time / 1.f;
	}


	m_fY = m_fOriginY;
	m_fY += ((1.f - fCoolTime) * m_fSizeY * 0.025f);


	VTXTEX vtx[] =
	{
		{ D3DXVECTOR3(-1.f, 1.f - (fCoolTime * 2.f), 0.f),D3DXVECTOR3(0.f, 1.f, 0.f) , D3DXVECTOR2(0.f, 0.f + fCoolTime) },
		{ D3DXVECTOR3(1.f , 1.f - (fCoolTime * 2.f), 0.f) , D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f , 0.f + fCoolTime) },
		{ D3DXVECTOR3(1.f , -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f , 1.f) },
		{ D3DXVECTOR3(-1.f, -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(0.f, 1.f) }
	};

	CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(this->m_pBuffer->m_VertexBuffer, 0, NULL, vtx, 0, 0);
}

