#include "stdafx.h"
#include "Font.h"
#include "Device.h"
#include <D2D1.h>
#include <DWrite.h>


CFont::CFont()
{
	m_pFW1FontWarpper = NULL;
	m_pFW1FontFactory = NULL;
	m_pDeivceContext = NULL;
	m_eType = FONT_TYPE_BASIC;
	m_wstrText = L"";
	m_fSize = 0.f;
	m_nColor = 0xFFFFFFFF;
	m_nFlag = FW1_LEFT | FW1_TOP | FW1_RESTORESTATE;
	m_vPos = D3DXVECTOR2(0.f, 0.f);
	m_fOutlineSize = 1.f;
	m_nOutlineColor = 0xFF000000;
	m_dwRefCount = 0;
	m_bEntered = false;
}

CFont::~CFont()
{
	Release();
}

CFont* CFont::Create(const wstring& _wstrPath)
{
	CFont* pResource = new CFont;
	if (FAILED(dynamic_cast<CFont*>(pResource)->Initialize(_wstrPath)))
		::Safe_Delete(pResource);

	return pResource;
}

CFont * CFont::CloneFont(void)
{

	CFont* pFont = this;

	++m_dwRefCount;

	return pFont;
}

HRESULT CFont::Initialize(const wstring& _wstrPath)
{
	m_pDeivceContext = CDevice::GetInstance()->m_pDeviceContext;
	Load_Font(_wstrPath);

	return S_OK;
}

HRESULT CFont::Load_Font(const wstring& _wstrPath)
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1FontFactory)))
	{
		MSG_BOX(L"FAILED CreateFactory");
		return E_FAIL;
	}

	if (FAILED(m_pFW1FontFactory->CreateFontWrapper(
		CDevice::GetInstance()->m_pDevice, _wstrPath.c_str(), &m_pFW1FontWarpper)))
	{
		MSG_BOX(L"Not Find Font");
		return E_FAIL;
	}

	return S_OK;
}

void CFont::Render()
{
	if (!m_pFW1FontWarpper)
		return;

	if (FONT_TYPE_OUTLINE == m_eType)
	{
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);

		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y,
			m_nOutlineColor, m_nFlag);
	}

	m_pFW1FontWarpper->DrawString(
		m_pDeivceContext,
		m_wstrText.c_str(), m_fSize,
		m_vPos.x, m_vPos.y,
		m_nColor, m_nFlag);
}


DWORD CFont::Release()
{
	if (m_dwRefCount == 0)
	{
		::Safe_Release(m_pFW1FontWarpper);
		::Safe_Release(m_pFW1FontFactory);
	}
	else
		--m_dwRefCount;

	return 0;
}
