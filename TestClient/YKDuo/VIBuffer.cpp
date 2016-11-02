#include "stdafx.h"
#include "VIBuffer.h"
#include "Device.h"
#include "Shader.h"
#include "ShaderMgr.h"


CVIBuffer::CVIBuffer()
{
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
	//pGrapicDevice->m_pDeviceContext->Draw(4, 0);

	//pGrapicDevice->m_pDeviceContext->RSSetState(m_pd3dRasterizerState);
	pGrapicDevice->m_pDeviceContext->DrawIndexed(m_iIndex, m_iStartIndex, m_iBaseVertex);
	//9에서 인덱스프리미티브 차이랑 같음

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
