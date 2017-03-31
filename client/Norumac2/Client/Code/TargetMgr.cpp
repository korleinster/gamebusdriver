#include "stdafx.h"
#include "TargetMgr.h"
#include "MultiRenderTarget.h"
#include "ShaderMgr.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTargetMgr)

CTargetMgr::CTargetMgr()
	: m_pMRT_GBuffer(NULL)
	, m_pGBufferVisVertexShader(NULL)
	, m_pGBufferVisPixelShader(NULL)
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

	m_pGBufferVisVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"GBufferVisVS");
	m_pGBufferVisPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"GBufferVisPS");

	return S_OK;
}

void CTargetMgr::Release()
{
	delete m_pMRT_GBuffer;
}

void CTargetMgr::RenderGBuffer(ID3D11DeviceContext* pd3dImmediateContext)
{
	
	ID3D11ShaderResourceView* arrViews[4] = 
	{
		m_pMRT_GBuffer->GetDepthView(),
		m_pMRT_GBuffer->GetColorView(), 
		m_pMRT_GBuffer->GetNormalView(),
		m_pMRT_GBuffer->GetSpecPowerView(),
	};
	// 이거모임 ㅋㅋ
	pd3dImmediateContext->RSSetState(NULL);

	pd3dImmediateContext->PSSetShaderResources(0, 4, arrViews);

	//pd3dImmediateContext->PSSetSamplers(0, 1, &g_pSampPoint);

	pd3dImmediateContext->IASetInputLayout(NULL);
	pd3dImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set the shaders
	pd3dImmediateContext->VSSetShader(m_pGBufferVisVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pGBufferVisPixelShader->m_pPixelShader, NULL, 0);

	// gpu에게 현재 버텍스버퍼, 버텍스레이아웃, 프리미티브 토폴로지를 사용해서 렌더링하라는 함수
	pd3dImmediateContext->Draw(
		16, // gpu로 보낸 버텍스의 갯수
		0); // 처음시작할 버텍스의 인덱스

	// Cleanup
	pd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);

	ZeroMemory(arrViews, sizeof(arrViews));
	pd3dImmediateContext->PSSetShaderResources(0, 4, arrViews);
	
	
}
