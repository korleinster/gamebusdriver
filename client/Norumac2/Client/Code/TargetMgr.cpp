#include "stdafx.h"
#include "TargetMgr.h"
#include "MultiRenderTarget.h"
#include "RenderTarget.h"
#include "LightMgr.h"

IMPLEMENT_SINGLETON(CTargetMgr)

CTargetMgr::CTargetMgr()
	: m_pMRT_GBuffer(NULL)
	, m_pMRT_Border(NULL)
	, m_pMRT_ShadowMap(NULL)
	//, m_pGBufferVisVertexShader(NULL)
	//, m_pGBufferVisPixelShader(NULL)
{
}


CTargetMgr::~CTargetMgr()
{
	Release();
}

HRESULT CTargetMgr::Initialize()
{
	m_pMRT_GBuffer = new CMultiRenderTarget;
	m_pMRT_GBuffer->Initialize(WINCX, WINCY);


	//GBuffer
	CRenderTarget* pRT_Color = CRenderTarget::Create(WINCX, WINCY, basicColorTextureFormat);
	pRT_Color->Ready_DebugBuffer(-0.5f, 0.8f, 0.1f, 0.1f, 0);
	CRenderTarget* pRT_Normal = CRenderTarget::Create(WINCX, WINCY, normalTextureFormat);
	pRT_Normal->Ready_DebugBuffer(-0.2f, 0.8f, 0.1f, 0.1f, 0);
	CRenderTarget* pRT_Spec = CRenderTarget::Create(WINCX, WINCY, specPowTextureFormat);
	pRT_Spec->Ready_DebugBuffer(0.1f, 0.8f, 0.1f, 0.1f, 0);

	m_pMRT_GBuffer->SetRT(pRT_Color);
	m_pMRT_GBuffer->SetRT(pRT_Normal);
	m_pMRT_GBuffer->SetRT(pRT_Spec);

	//Border
	m_pMRT_Border = new CMultiRenderTarget;
	m_pMRT_Border->Initialize(WINCX, WINCY);

	CRenderTarget* pRT_Border = CRenderTarget::Create(WINCX, WINCY, basicColorTextureFormat);
	pRT_Border->Ready_DebugBuffer(-0.8f, 0.5f, 0.1f, 0.1f, 0);
	m_pMRT_Border->SetRT(pRT_Border);

	//ShadowMap
	m_pMRT_ShadowMap = new CMultiRenderTarget;
	m_pMRT_ShadowMap->Initialize(WINCX, WINCY);

	CRenderTarget* pRT_ShadowMap = CRenderTarget::Create(WINCX, WINCY, basicColorTextureFormat);
	pRT_ShadowMap->Ready_DebugBuffer(-0.5f, 0.5f, 0.1f, 0.1f, 0);
	m_pMRT_ShadowMap->SetRT(pRT_ShadowMap);

	return S_OK;
}

void CTargetMgr::Release()
{
	Safe_Delete(m_pMRT_GBuffer);
	Safe_Delete(m_pMRT_Border);
	Safe_Delete(m_pMRT_ShadowMap);
	//delete m_pMRT_GBuffer;
	//delete m_pMRT_Border;
	//delete m_pMRT_ShadowMap;
}

void CTargetMgr::RenderGBuffer(ID3D11DeviceContext* pd3dImmediateContext)
{
	m_pMRT_GBuffer->RenderDepth(pd3dImmediateContext);
	m_pMRT_GBuffer->RenderMRT(pd3dImmediateContext);
	m_pMRT_Border->RenderMRT(pd3dImmediateContext);
	m_pMRT_ShadowMap->RenderMRT(pd3dImmediateContext, CLightMgr::GetInstance()->GetShadowSRV());
}
