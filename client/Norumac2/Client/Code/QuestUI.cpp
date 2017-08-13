#include "stdafx.h"
#include "QuestUI.h"

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
#include "Input.h"
#include "TimeMgr.h"
#include "SoundMgr.h"

CQuestUI::CQuestUI()
{
	m_bRender = false;
	m_fCoolTime = 0.f;
}

CQuestUI::~CQuestUI()
{
	CObj::Release();
}

HRESULT CQuestUI::Initialize(void)
{
	FAILED_CHECK(AddComponent());

	m_fX = 900.f;
	m_fY = 350.f;
	m_fSizeX = 140.f;
	m_fSizeY = 210.f;


	m_QuestScript->m_eType = FONT_TYPE_OUTLINE;

	m_QuestScript->m_wstrText = L" ";
	m_QuestScript->m_fSize = 20.f;
	m_QuestScript->m_nColor = 0xFFFFFFFF;
	m_QuestScript->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_QuestScript->m_vPos = D3DXVECTOR2(m_fX - 100.f, m_fY - 100.f);
	m_QuestScript->m_fOutlineSize = 1.f;
	m_QuestScript->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;



	m_QuestState->m_eType = FONT_TYPE_OUTLINE;

	m_QuestState->m_wstrText = L" ";
	m_QuestState->m_fSize = 18.f;
	m_QuestState->m_nColor = 0xFFFFFFFF;
	m_QuestState->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_QuestState->m_vPos = D3DXVECTOR2(m_fX - 100.f, m_fY+ 100.f);
	m_QuestState->m_fOutlineSize = 1.f;
	m_QuestState->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;	

	//
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);
	return S_OK;
}

int CQuestUI::Update(void)
{
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matView);
	m_matView._11 = m_fSizeX;
	m_matView._22 = m_fSizeY;
	m_matView._33 = 1.f;

	// -0.5	-> -400		,	0.5	-> 400
	m_matView._41 = m_fX - (WINCX >> 1);
	m_matView._42 = -m_fY + (WINCY >> 1);


	m_fCoolTime += CTimeMgr::GetInstance()->GetTime();

	if ((CInput::GetInstance()->GetDIKeyState(DIK_M) & 0x80) && g_bChatMode == false)
	{
		if (m_bRender == true && m_fCoolTime > 0.2f)
		{
			CSoundMgr::GetInstance()->PlayInterface1(L"WindowClose.ogg");
			m_bRender = false;
			m_fCoolTime = 0.f;
		}
		else if (m_bRender == false && m_fCoolTime > 0.2f)
		{
			CSoundMgr::GetInstance()->PlayInterface1(L"WindowOpen.ogg");
			m_bRender = true;
			m_fCoolTime = 0.f;
		}
	}


	CObj::Update();


	return 0;

}

void CQuestUI::Render()
{
	ConstantBuffer cb;

	if (m_bRender)
	{
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

		m_QuestScript->Render();
		m_QuestState->Render();
	}
}

CQuestUI * CQuestUI::Create(void)
{
	CQuestUI* pUI = new CQuestUI;

	if (FAILED(pUI->Initialize()))
	{
		::Safe_Delete(pUI);
	}

	return pUI;
}

HRESULT CQuestUI::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_QuestUI");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	m_QuestScript = CFont::Create(L"Font_Star");
	m_QuestState = CFont::Create(L"Font_Star");

	return S_OK;
}

