#include "stdafx.h"
#include "LineCol.h"
#include "Device.h"
#include "NaviCell.h"

CLineCol::CLineCol(CNaviCell* pNaviCell)
{
	m_idx = NULL;
	m_vCol = NULL;
}
CLineCol::~CLineCol(void)
{
	Release();
}

HRESULT CLineCol::CreateBuffer(CNaviCell* pNeviCell)
{

	HRESULT hr = E_FAIL;
	m_iVertices = 3;
	m_iVertexStrides = sizeof(VTXCOL);
	m_eDrawType = DRAW_LINE;

	m_iIndex = 6;
	m_iStartIndex = 0;
	m_iBaseVertex = 0;

	m_vCol = new VTXCOL[m_iVertexStrides];

	int iIndex = 0;


	m_vCol[0].vPos = *((pNeviCell)->GetPoint(CNaviCell::POINT_A));
	if ((pNeviCell)->GetType() == TYPE_TERRAIN)
		m_vCol[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	else if ((pNeviCell)->GetType() == TYPE_MESH)
		m_vCol[0].dwColor = D3DXCOLOR(0.f, 1.f, 1.f, 1.f);

	m_vCol[1].vPos = *((pNeviCell)->GetPoint(CNaviCell::POINT_B));
	if ((pNeviCell)->GetType() == TYPE_TERRAIN)
		m_vCol[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	else if ((pNeviCell)->GetType() == TYPE_MESH)
		m_vCol[1].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	m_vCol[2].vPos = *((pNeviCell)->GetPoint(CNaviCell::POINT_C));
	if ((pNeviCell)->GetType() == TYPE_TERRAIN)
		m_vCol[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	else if ((pNeviCell)->GetType() == TYPE_MESH)
		m_vCol[2].dwColor = D3DXCOLOR(0.f, 1.f, 1.f, 1.f);



	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_iVertexStrides * m_iVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = m_vCol;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&bd, &InitData, &m_VertexBuffer);

	m_idx = new INDEX16[m_iIndex];


	m_idx[0]._1 = 0;
	m_idx[1]._2 = 1;
	m_idx[2]._3 = 2;




	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_iIndex;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = m_idx;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_IndexBuffer);

	//ConstantBuffer
	D3D11_BUFFER_DESC cb;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(ConstantBuffer);
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cb, NULL, &m_ConstantBuffer);


	CreateRasterizerStateLine();



	return S_OK;
}

CLineCol* CLineCol::Create(CNaviCell* pNaviCell)
{
	CLineCol* pLineCol = new CLineCol(pNaviCell);
	if (FAILED(pLineCol->CreateBuffer(pNaviCell)))
		Safe_Delete(pLineCol);

	return pLineCol;
}

CResources* CLineCol::CloneResource()
{
	CResources* pResource = this;

	pResource->AddRef();

	return pResource;
}

DWORD CLineCol::Release()
{
	if (m_dwRefCount == 0)
	{
		Safe_Delete_Array(m_vCol);
		Safe_Delete_Array(m_idx);
	}
	else
		--m_dwRefCount;

	return m_dwRefCount;
}