#include "stdafx.h"
#include "RcTerrain.h"
#include "Device.h"


CRcTerrain::CRcTerrain()
{
}


CRcTerrain::~CRcTerrain()
{
}

HRESULT CRcTerrain::CreateBuffer(UINT iCountX, UINT iCountZ, UINT iInterval)
{
	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXTEX);
	m_iVertexOffsets = 0;


	DWORD*		pdwPixel = LoadImage();

	UINT iVertexCount = iCountX * iCountZ;

	VTXTEX* Vertex = new VTXTEX[iVertexCount];

	int		iIndex = 0;

	for (int z = 0; z < iCountZ; ++z)
	{
		for (int x = 0; x < iCountX; ++x)
		{
			iIndex = z * iCountX + x;

			Vertex[iIndex].vPos = D3DXVECTOR3(float(x) * iInterval, (pdwPixel[iIndex] & 0x000000ff) / 100.f, float(z) * iInterval);
			Vertex[iIndex].vTexUV = D3DXVECTOR2(x / (iCountX - 1.f), z / (iCountZ - 1.f));
		}
	}




	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VTXTEX) * iVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData;
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = Vertex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);


	::Safe_Delete_Array(Vertex);

	if (FAILED(hr))
		return E_FAIL;


	//int AAAA = (iCountX - 1) * (iCountZ - 1) * 2 + 1;
	m_iIndex = (iCountX - 1) * (iCountZ - 1) * 2;

	INDEX32* Index = new INDEX32[m_iIndex];

	int	iTriCnt = 0;

	for (int z = 0; z < iCountZ - 1; ++z)
	{
		for (int x = 0; x < iCountX - 1; ++x)
		{
			iIndex = z * iCountX + x;

			Index[iTriCnt]._1 = iIndex + iCountX;
			Index[iTriCnt]._2 = iIndex + iCountX + 1;
			Index[iTriCnt]._3 = iIndex + 1;

			++iTriCnt;

			Index[iTriCnt]._1 = iIndex + iCountX;
			Index[iTriCnt]._2 = iIndex + 1;
			Index[iTriCnt]._3 = iIndex;

			++iTriCnt;
		}
	}
	//32768 * 

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(INDEX32) * m_iIndex;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = Index;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	::Safe_Delete_Array(Index);

	if (FAILED(hr))
		return E_FAIL;


	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DYNAMIC;
	//bd.ByteWidth = sizeof(ConstantBuffer);
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//CDevice::GetInstance()->m_pDevice->CreateBuffer(&bd, NULL, &m_ConstantBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;

	}

	m_iIndex *= 3;
	return S_OK;
}

CRcTerrain* CRcTerrain::Create(UINT iCountX, UINT iCountZ, UINT iInterval)
{
	CRcTerrain* pTerrain = new CRcTerrain;

	if (FAILED(pTerrain->CreateBuffer(iCountX, iCountZ, iInterval)))
	{
		::Safe_Delete(pTerrain);
	}
	return pTerrain;
}

DWORD CRcTerrain::Release(void)
{
	return 0;
}

CResources * CRcTerrain::CloneResource(void)
{
	return nullptr;
}

DWORD * CRcTerrain::LoadImage(void)
{
	HANDLE		hFile = NULL;
	DWORD		dwByte = 0;

	hFile = CreateFile(L"../Resource/Terrain/Height.bmp", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER			fh;
	BITMAPINFOHEADER			ih;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, NULL);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, NULL);

	DWORD*		pdwPixel = new DWORD[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pdwPixel, sizeof(DWORD) * ih.biWidth * ih.biHeight, &dwByte, NULL);


	CloseHandle(hFile);

	return pdwPixel;
}
