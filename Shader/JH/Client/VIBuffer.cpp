#include "stdafx.h"
#include "VIBuffer.h"
#include "Device.h"
#include "Shader.h"
#include "ShaderMgr.h"


CVIBuffer::CVIBuffer()
{
	m_pRasterizerState = nullptr;
	m_VertexBuffer = NULL;
	m_IndexBuffer = NULL;
	m_ConstantBuffer = NULL;

	m_iVertices = 0;
	m_iBuffers = 0;
	m_iSlot = 0;
	m_iStartVertex = 0;
	m_iVertexStrides = 0;
	m_iVertexOffsets = 0;
	m_iIndex = 0;
	m_iStartIndex = 0;
	m_iBaseVertex = 0;
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
{
	m_VertexBuffer = rhs.m_VertexBuffer;
	m_IndexBuffer = rhs.m_IndexBuffer;
	m_ConstantBuffer = rhs.m_ConstantBuffer;

	m_iVertices = rhs.m_iVertices;
	m_iBuffers = rhs.m_iBuffers;
	m_iSlot = rhs.m_iSlot;
	m_iStartVertex = m_iStartVertex;
	m_iVertexStrides = m_iVertexStrides;
	m_iVertexOffsets = m_iVertexOffsets;
	m_iIndex = m_iIndex;
	m_iStartIndex = m_iStartIndex;
	m_iBaseVertex = m_iBaseVertex;
}


CVIBuffer::~CVIBuffer()
{

}

void CVIBuffer::Render(void)
{
	CDevice* pGrapicDevice = CDevice::GetInstance();
	
	
	pGrapicDevice->m_pDeviceContext->IASetVertexBuffers(m_iSlot, 1, &m_VertexBuffer, &m_iVertexStrides, &m_iVertexOffsets);
	//버텍스

	pGrapicDevice->m_pDeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//인덱스 

	pGrapicDevice->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pGrapicDevice->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//pGrapicDevice->m_pDeviceContext->Draw(4, 0);

	if (m_pRasterizerState)
		pGrapicDevice->m_pDeviceContext->RSSetState(m_pRasterizerState);

	//pGrapicDevice->m_pDeviceContext->RSSetState(m_pd3dRasterizerState);
	pGrapicDevice->m_pDeviceContext->DrawIndexed(m_iIndex, m_iStartIndex, m_iBaseVertex);
	pGrapicDevice->m_pDeviceContext->Draw(m_iVertices, m_iStartVertex);
	//9에서 인덱스프리미티브 차이랑 같음

}

void CVIBuffer::CreateRasterizerState()
{
	// 레스터 라이저에서 다음과 같은 처리를 수행한다
	// 보이지 않는 프리미티브 제거(은면제거, 컬링 처리)
	// 좌표의 뷰포트 변환
	// 시저 테스트
	// 깊이 바이어스 계산
	// 프리미티브들을 렌더 타겟 상의 텍셀 단위로 변경
	// 멀티 샘플링이나 필 모드의 설정에 따른 처리
	CDevice* pGrapicDevice = CDevice::GetInstance();
	
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID; // 기본의 고형체 렌더링 와이어프레임은 D3D11_FILL_WIREFRAME
	tRasterizerDesc.CullMode = D3D11_CULL_BACK; // 후면 삼각형을 선별해서 제외 시킴
	tRasterizerDesc.FrontCounterClockwise = false; // 카메라기준 시계방향으로 감긴 삼각형을 전면으로 간주
	tRasterizerDesc.DepthBias = 0; // 깊이 바이어스 값
	tRasterizerDesc.DepthBiasClamp = 0.f;
	tRasterizerDesc.SlopeScaledDepthBias = 0.f;
	tRasterizerDesc.DepthClipEnable = true; // 깊이 클리핑 
	tRasterizerDesc.ScissorEnable = false; // 시저 테스트
	tRasterizerDesc.MultisampleEnable = false; // 멀티 샘플링
	tRasterizerDesc.AntialiasedLineEnable = false;	// 라인 안티 앨리어싱
	
	pGrapicDevice->m_pDevice->CreateRasterizerState(
		&tRasterizerDesc, // 생성하고자 하는 래스터화기 상태 집합을 서술하는 D3D11_RASTERIZER_DESC 구조체
		&m_pRasterizerState); // ID3D11RasterizerState를 가리키는 포인터
}

DWORD CVIBuffer::Release(void)
{
	if (m_dwRefCount == 2)
	{
		Safe_Release(m_VertexBuffer);
		Safe_Release(m_IndexBuffer);
		Safe_Release(m_ConstantBuffer);
	}
	return 0;
}
