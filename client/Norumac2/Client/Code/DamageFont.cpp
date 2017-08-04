#include "stdafx.h"
#include "Include.h"
#include "UI.h"
#include "DamageFont.h"
#include "TimeMgr.h"
#include "Device.h"
#include "Camera.h"
#include "Font.h"
#include "RenderMgr.h"

CDamageFont::CDamageFont()
	: CUI()
	, m_vPoint(D3DXVECTOR3(0.f, 0.f, 0.f))
	, m_fTime(0.f)
	, m_fAlpha(1.f)
	, m_fMoveSize(0.f)
{

}

CDamageFont::~CDamageFont(void)
{
	CObj::Release();
	CRenderMgr::GetInstance()->DelRenderGroup(TYPE_UI, this);//임시인데 어디다 빼야할까
}

HRESULT CDamageFont::Initialize(const D3DXVECTOR3* pPos, const float& fDmg, const float& fFontSize, const UINT32& nColor)
{
	if (FAILED(Add_Component()))
		return E_FAIL;



	D3DXMATRIX matView = CCamera::GetInstance()->m_matView;
	D3DXMATRIX matProj = CCamera::GetInstance()->m_matProj;

	D3DXVec3TransformCoord(&m_vPoint, pPos, &matView);
	D3DXVec3TransformCoord(&m_vPoint, &m_vPoint, &matProj);

	m_vPoint.x = (m_vPoint.x + 1) * WINCX / 2.f;
	m_vPoint.y = (-m_vPoint.y + 1)* WINCY / 2.f;
	//스크린 좌표까지 올려준다.

	m_pFont->m_eType = FONT_TYPE_OUTLINE;

	m_pFont->m_wstrText = to_wstring((int)fDmg);
	m_pFont->m_fSize = fFontSize;
	m_pFont->m_nColor = nColor;
	m_pFont->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(m_vPoint.x, m_vPoint.y - 30.f);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000 /*0xFFFFFFFF*/;


	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);

	return S_OK;
}

int CDamageFont::Update(void)
{
	m_fTime += CTimeMgr::GetInstance()->GetTime();

	if (m_fTime > 1.3f)
		m_bDeath = true;

	m_pFont->m_vPos.y -= 20.f * CTimeMgr::GetInstance()->GetTime();

	if (!m_bDeath)
		return 0;

	else
		return 100;



	return 0;
}

void CDamageFont::Render(void)
{
	m_pFont->Render();
}

CDamageFont* CDamageFont::Create(const D3DXVECTOR3* pPos, const float& fDmg, const float& fFontSize, const UINT32& nColor)
{
	CDamageFont*		pDmgFont = new CDamageFont();

	if (FAILED(pDmgFont->Initialize(pPos, fDmg,fFontSize,nColor)))
	{
		//MSG_BOX("CDamageFont Created Failed");
		::Safe_Delete(pDmgFont);
	}

	return pDmgFont;
}

HRESULT CDamageFont::Add_Component(void)
{
	m_pFont = CFont::Create(L"Font_Clear");
	return S_OK;
}
