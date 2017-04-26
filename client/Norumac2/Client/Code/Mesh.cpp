#include "stdafx.h"
#include "Mesh.h"
#include "Shader.h"
#include "Device.h"


CMesh::CMesh()
	: m_pShader(NULL),
	m_pGrapicDevice(CDevice::GetInstance())
{
}


CMesh::~CMesh()
{
}

void CMesh::Render()
{
	CVIBuffer::Render();
}

void CMesh::MakeVertexNormal(BYTE * _pVertices, WORD * _pIndices)
{
	if (!_pIndices)
		SetNormalVectorByBasic(_pVertices);
	else
		SetNormalVectorByAverage(_pVertices, _pIndices, (m_iIndex / 3), 3, false);
}

void CMesh::SetNormalVectorByBasic(BYTE * _pVertices)
{
	D3DXVECTOR3		vNormal;
	VTXTEX*	pVertex = NULL;

	int nPrimitives = m_iVertices / 3;
	for (int i = 0; i < nPrimitives; ++i)
	{
		vNormal = GetTriAngleNormal(_pVertices, (i * 3 + 0), (i * 3 + 1), (i * 3 + 2));

		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 0) * m_iVertexStrides));
		pVertex->vNormal = -vNormal;
		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 1) * m_iVertexStrides));
		pVertex->vNormal = -vNormal;
		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 2) * m_iVertexStrides));
		pVertex->vNormal = -vNormal;
	}
}

void CMesh::SetNormalVectorByAverage(BYTE * _pVertices, WORD * _pIndices, int _iPrimitives, int _iOffset, bool _bStrip)
{
	D3DXVECTOR3		vNormal(0.f, 0.f, 0.f);
	VTXTEX*	pVertex = NULL;
	USHORT nIndex_0, nIndex_1, nIndex_2;

	for (int i = 0; i < m_iVertices; ++i)
	{
		vNormal = D3DXVECTOR3(0.f, 0.f, 0.f);

		for (int j = 0; j < _iPrimitives; j++)
		{
			nIndex_0 = (_bStrip) ? (((j % 2) == 0) ? (j * _iOffset + 0) : (j * _iOffset + 1)) : (j * _iOffset + 0);
			if (_pIndices) nIndex_0 = _pIndices[nIndex_0];

			nIndex_1 = (_bStrip) ? (((j % 2) == 0) ? (j * _iOffset + 1) : (j * _iOffset + 0)) : (j * _iOffset + 1);
			if (_pIndices) nIndex_1 = _pIndices[nIndex_1];

			nIndex_2 = (_pIndices) ? _pIndices[j * _iOffset + 2] : (j * _iOffset + 2);

			if ((nIndex_0 == i) || (nIndex_1 == i) || (nIndex_2 == i))
				vNormal += GetTriAngleNormal(_pVertices, nIndex_0, nIndex_1, nIndex_2);
		}
		D3DXVec3Normalize(&vNormal, &vNormal);
		pVertex = (VTXTEX *)(_pVertices + (i * m_iVertexStrides));
		pVertex->vNormal = vNormal;
	}
}

D3DXVECTOR3 CMesh::GetTriAngleNormal(BYTE * _pVertices, USHORT _nIndex_0, USHORT _nIndex_1, USHORT _nIndex_2)
{
	D3DXVECTOR3 vNormal(0.f, 0.f, 0.f);
	D3DXVECTOR3 vP_0 = *((D3DXVECTOR3 *)(_pVertices + (m_iVertexStrides * _nIndex_0)));
	D3DXVECTOR3 vP_1 = *((D3DXVECTOR3 *)(_pVertices + (m_iVertexStrides * _nIndex_1)));
	D3DXVECTOR3 vP_2 = *((D3DXVECTOR3 *)(_pVertices + (m_iVertexStrides * _nIndex_2)));

	D3DXVECTOR3 vEdge_0 = vP_1 - vP_0;
	D3DXVECTOR3 vEdge_1 = vP_2 - vP_0;

	D3DXVec3Cross(&vNormal, &vEdge_0, &vEdge_1);
	D3DXVec3Normalize(&vNormal, &vNormal);

	return vNormal;
}
