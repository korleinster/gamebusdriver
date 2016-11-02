#include "stdafx.h"
#include "CylinderCol.h"
#include "Device.h"
#include "Include.h"

CCylinderCol::CCylinderCol()
{
}


CCylinderCol::~CCylinderCol()
{

}

HRESULT CCylinderCol::CreateBuffer(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount)
{
	m_meshData.Vertices.clear();
	m_meshData.Indices.clear();


	BuildCylinder(fBottomRadius, fTopRadius, fHeight, iSliceCount, iStackCount, m_meshData);
	BuildCylinderTopCap(fBottomRadius, fTopRadius, fHeight, iSliceCount, iStackCount, m_meshData);
	BuildCylinderBottomCap(fBottomRadius, fTopRadius, fHeight, iSliceCount, iStackCount, m_meshData);
	//버텍스와 인덱스 설정해주는것

	
	HRESULT hr = E_FAIL;

	m_iVertexOffsets = 0;
	m_iCylinderIndexCnt = m_meshData.Indices.size();
	m_iCylinderVertexCnt = m_meshData.Vertices.size();
	m_iIndexOffset = sizeof(VTXCYL);
	
	vector<VTXCYL> vertices(m_iCylinderIndexCnt);

	UINT k = 0;

	for (size_t i = 0; i < m_meshData.Vertices.size(); ++i, ++k)
	{
		vertices[k].vPos = m_meshData.Vertices[i].Position;
		vertices[k].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VTXCYL) * m_iCylinderVertexCnt;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData; // 데이터->버퍼의 실질적인 데이터(좌표/색상)값을 들고 있는 데이터
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = &vertices[0];
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);

	if (FAILED(hr))
	{
		MSG_BOX(L"Failed");
		return E_FAIL;
	}

	vector<UINT> indeces;
	indeces.insert(indeces.end(), m_meshData.Indices.begin(), m_meshData.Indices.end());
	//인덱스 순서

	m_iIndex = 3 * m_iCylinderIndexCnt;

	D3D11_BUFFER_DESC ibd;//인덱스버퍼
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * m_iCylinderIndexCnt;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indeces[0];
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	if (FAILED(hr))
		return E_FAIL;

	D3D11_BUFFER_DESC cbd;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;

	}

	return S_OK;


}
void CCylinderCol::BuildCylinder(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount, MeshData& meshData)
{
	

	float stackHeight = fHeight / iStackCount;

	float radiusStep = (fTopRadius - fBottomRadius) / iStackCount;

	UINT ringCount = iStackCount + 1;

	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * fHeight + i * stackHeight;
		float r = fBottomRadius + i * radiusStep;

		float dTheta = 2.0f * D3DX_PI / iSliceCount;
		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			CylinderVertex vertex;
			float c = 0.f;
			float s = 0.f;
			float radian = j * dTheta;
			//float temp = D3DXToDegree(radian);
			c = cosf(radian);
			s = sinf(radian);

			if (s == 1)
				c = 0;
			if (c == 1)
				s = 0;
			if (s == -1)
				c = 0;
			if (c == -1)
				s = 0;

			vertex.Position = D3DXVECTOR3(r*c, y, r*s);

			vertex.TexC.x = (float)j / iSliceCount;
			vertex.TexC.y = 1.0f - (float)i / iStackCount;

			vertex.TangentU = D3DXVECTOR3(-s, 0.0f, c);

			float dr = fBottomRadius - fTopRadius;
			D3DXVECTOR3 bitangent(dr*c, -fHeight, dr*s);

			D3DXVECTOR3 T = vertex.TangentU;
			D3DXVECTOR3 B = bitangent;
			D3DXVECTOR3 N;
			D3DXVec3Cross(&N, &T, &B);
			D3DXVec3Normalize(&N, &N);
			vertex.Normal = N;

			meshData.Vertices.push_back(vertex);


		}
	}

	UINT ringVertexCount = iSliceCount + 1;

	for (UINT i = 0; i < iStackCount; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)* ringVertexCount + j + 1);

			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);
			meshData.Indices.push_back(i*ringVertexCount + j + 1);

		}
	}
}

void CCylinderCol::BuildCylinderTopCap(float fBottomRadius, float fTopRadius, float fHeight,
	UINT iSliceCount, UINT iStackCount, MeshData& meshData)
{
	UINT baseIndex = (UINT)meshData.Vertices.size();

	float y = 0.5f * fHeight;
	float dTheta = 2.0f * D3DX_PI / iSliceCount;

	for (UINT i = 0; i <= iSliceCount; ++i)
	{
		float c = cosf(i*dTheta);
		float s = sinf(i*dTheta);

		if (s == 1)
			c = 0;
		if (c == 1)
			s = 0;
		if (s == -1)
			c = 0;
		if (c == -1)
			s = 0;

		float x = fTopRadius * c;
		float z = fTopRadius * s;

		float u = x / fHeight + 0.5f;
		float v = z / fHeight + 0.5f;

		meshData.Vertices.push_back(CylinderVertex(D3DXVECTOR3(x, y, z),D3DXVECTOR3(0.0f, 1.0f, 0.0f)
			, D3DXVECTOR3(0.0f, 0.0f, 0.0f),D3DXVECTOR2(u, v)));
	}

	meshData.Vertices.push_back(CylinderVertex(D3DXVECTOR3 (0.0f, y, 0.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		D3DXVECTOR2(0.5f, 0.5f)));

	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < iSliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i + 1);
		meshData.Indices.push_back(baseIndex + i);
	}

}

void CCylinderCol::BuildCylinderBottomCap(float fBottomRadius, float fTopRadius, float fHeight,
	UINT iSliceCount, UINT iStackCount, MeshData& meshData)
{
	UINT baseIndex = (UINT)meshData.Vertices.size();
	float y = -0.5f * fHeight;

	float dTheta = 2.0f * D3DX_PI / iSliceCount;
	for (UINT i = 0; i <= iSliceCount; ++i)
	{
		float c = cosf(i*dTheta);
		float s = sinf(i*dTheta);

		if (s == 1)
			c = 0;
		if (c == 1)
			s = 0;
		if (s == -1)
			c = 0;
		if (c == -1)
			s = 0;

		float x = fTopRadius * c;
		float z = fTopRadius * s;

		float u = x / fHeight + 0.5f;
		float v = z / fHeight + 0.5f;

		meshData.Vertices.push_back(CylinderVertex(D3DXVECTOR3(x, y, z), 
			D3DXVECTOR3(0.0f, -1.0f, 0.0f), 
			D3DXVECTOR3 (1.0f, 0.0f, 0.0f), 
			D3DXVECTOR2 (u, v)));
	}

	meshData.Vertices.push_back(CylinderVertex(D3DXVECTOR3(0.0f, y, 0.0f), 
		D3DXVECTOR3(0.0f, -1.0f, 0.0f), 
		D3DXVECTOR3(1.0f, 0.0f, 0.0f), 
		D3DXVECTOR2(0.5f, 0.5f)));

	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < iSliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}
}

CCylinderCol* CCylinderCol::Create(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount)
{
	CCylinderCol*	pCylinderCol = new CCylinderCol;

	if (FAILED(pCylinderCol->CreateBuffer(fBottomRadius,fTopRadius,fHeight,iSliceCount,iStackCount)))
		Safe_Delete(pCylinderCol);

	return pCylinderCol;
}

CResources* CCylinderCol::CloneResource(void)
{
	CResources* pResource = new CCylinderCol(*this);

	pResource->AddRef();

	return pResource;
}
