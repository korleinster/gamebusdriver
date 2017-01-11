#include "stdafx.h"
#include "RenderMgr.h"
#include "Obj.h"
#include "Scene.h"
#include "Device.h"
#include "TargetMgr.h"
#include "LightMgr.h"
#include "MultiRenderTarget.h"

IMPLEMENT_SINGLETON(CRenderMgr)

CRenderMgr::CRenderMgr()
	: m_fTime(0.f)
	, m_dwCount(0)
	, m_pDevice(CDevice::GetInstance())
	, m_pTargetMgr(CTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
{
	ZeroMemory(m_szFps, sizeof(TCHAR) * 128);
	m_pTargetMgr->Initialize();
	m_pLightMgr->Initialize();

	m_pLightMgr->SetAmbient(D3DXVECTOR3(0.1f, 0.2f, 0.1f), D3DXVECTOR3(0.1f, 0.2f, 0.2f));
	m_pLightMgr->SetDirectional(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.8f, 0.8f, 0.0f));
}


CRenderMgr::~CRenderMgr()
{
	m_pTargetMgr->DestroyInstance();
	m_pLightMgr->DestroyInstance();
}

//void CRenderMgr::SetCurrentScene(CScene * pScene)
//{
//}

void CRenderMgr::AddRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	NULL_CHECK_RETURN(pObj, );

	m_RenderGroup[eRednerID].push_back(pObj);
}

HRESULT CRenderMgr::InitScene(void)
{
	return S_OK;
}

void CRenderMgr::Render(const float & fTime)
{
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	m_pDevice->m_pDeviceContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set render resources
	//m_pDevice->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSampLinear);

	// GBuffer
	m_pTargetMgr->GetGBuffer()->Begin_MRT(m_pDevice->m_pDeviceContext);

	Render_Priority();
	Render_NoneAlpha();

	m_pTargetMgr->GetGBuffer()->End_MRT(m_pDevice->m_pDeviceContext);

	// Set the render target and do the lighting
	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthReadOnlyDSV());
	
	m_pTargetMgr->GetGBuffer()->PrepareForUnpack(m_pDevice->m_pDeviceContext);
	m_pLightMgr->DoLighting(m_pDevice->m_pDeviceContext, m_pTargetMgr->GetGBuffer());
	
	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, NULL);
	m_pTargetMgr->RenderGBuffer(m_pDevice->m_pDeviceContext);
	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthDSV());

	m_pDevice->m_pDeviceContext->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	Safe_Release(pPrevDepthState);

	Render_Alpha();
	Render_UI();
	Render_FPS(fTime);
}

void CRenderMgr::Render_Priority(void)
{
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_PRIORITY].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_PRIORITY].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}

	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderMgr::Render_NoneAlpha(void)
{
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_NONEALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_NONEALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

bool Compare_ViewZ(CObj* pSour, CObj* pDest)
{
	return pSour->GetViewZ() > pDest->GetViewZ();
}

void CRenderMgr::Render_Alpha(void)
{
	m_RenderGroup[TYPE_ALPHA].sort(Compare_ViewZ);

	// 	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// 	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// 
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_ALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_ALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderMgr::Render_UI(void)
{
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_UI].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_UI].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderMgr::Render_FPS(const float & fTime)
{
	m_fTime += fTime;
	++m_dwCount;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFps, L"FPS : %d", m_dwCount);
		m_fTime = 0.f;
		m_dwCount = 0;
	}
	::SetWindowText(g_hWnd, m_szFps);
}

void CRenderMgr::Release(void)
{
}

void CRenderMgr::ListClear(void)
{
	for (UINT i = 0; i < TYPE_END; ++i)
	{
		m_RenderGroup[i].clear();
	}
}
