#include "stdafx.h"
#include "RenderMgr.h"
#include "Obj.h"
#include "Scene.h"

IMPLEMENT_SINGLETON(CRenderMgr)

CRenderMgr::CRenderMgr()
	:m_fTime(0.f)
	, m_dwCount(0)
{
	ZeroMemory(m_szFps, sizeof(TCHAR) * 128);
}


CRenderMgr::~CRenderMgr()
{
}

void CRenderMgr::SetCurrentScene(CScene * pScene)
{
}

void CRenderMgr::AddRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	NULL_CHECK_RETURN(pObj, );

	m_RenderGroup[eRednerID].push_back(pObj);
}

void CRenderMgr::DelRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	list<CObj*>::iterator	iter = m_RenderGroup[eRednerID].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[eRednerID].end();

	for (; iter != iter_end; )
	{
		if ((*iter) == pObj)
			m_RenderGroup[eRednerID].erase(iter++);
		else
			++iter;

	}
}

HRESULT CRenderMgr::InitScene(void)
{
	return S_OK;
}

void CRenderMgr::Render(const float & fTime)
{
	Render_Priority();
	Render_NoneAlpha();
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
		if ((*iter) == NULL)
			m_RenderGroup[TYPE_PRIORITY].erase(iter++);
		else
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
		if ((*iter) == NULL)
			m_RenderGroup[TYPE_NONEALPHA].erase(iter++);
		else
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
		if ((*iter) == NULL)
			m_RenderGroup[TYPE_ALPHA].erase(iter++);
		else
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
		if ((*iter) == NULL)
			m_RenderGroup[TYPE_UI].erase(iter++);
		else
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
