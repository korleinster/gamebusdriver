#include "stdafx.h"
#include "RcTerrain.h"
#include "Device.h"
#include "ParsingDevice9.h"
#include "Vibuffer.h"


CRcTerrain::CRcTerrain()
{
}


CRcTerrain::~CRcTerrain()
{
}

HRESULT CRcTerrain::CreateBuffer(UINT iCountX, UINT iCountZ, UINT iInterval)
{
	CVIBuffer::CreateRasterizerState();

	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXTEX);
	m_iVertexOffsets = 0;

	LoadImage();

	D3DSURFACE_DESC pSurface;
	D3DLOCKED_RECT pD3D_Rect;

	m_pTexHeightMap->GetLevelDesc(0, &pSurface);

	UINT iVertexCount = iCountX * iCountZ;

	Vertex = new VTXTEX[iVertexCount];

	iCountZ = pSurface.Height;
	iCountX = pSurface.Width;


	m_pTexHeightMap->LockRect(0, &pD3D_Rect, NULL, D3DLOCK_READONLY);
	int		iIndex = 0;

	for (int z = 0; z < iCountZ; ++z)
	{
		for (int x = 0; x < iCountX; ++x)
		{
			//iIndex = z * iCountX + x;

			//Vertex[iIndex].vPos		= D3DXVECTOR3(float(x) * iInterval,(pdwPixel[iIndex] & 0x000000ff) / 100.f,	float(z) * iInterval);
			//Vertex[iIndex].vTexUV  = D3DXVECTOR2(x / (iCountX - 1.f), z / (iCountZ - 1.f));

			iIndex = z * iCountX + x;

			float fX = float(x) * iInterval;
			float fY = 0.f;
			float fZ = float(z) * iInterval;

			fY = (float(*((LPDWORD)pD3D_Rect.pBits + x + z * (pD3D_Rect.Pitch / 4)) & 0x000000ff)) / 9.f;

			//Vertex[iIndex].vPos = D3DXVECTOR3(fX, fY, fZ);
			//Vertex[iIndex].vTex = D3DXVECTOR2(x / (dwCntX - 1.f), z / (dwCntY - 1.f));

			Vertex[iIndex].vPos = D3DXVECTOR3(fX, fY, fZ);
			Vertex[iIndex].vTexUV = D3DXVECTOR2(x / (iCountX - 1.f), z / (iCountZ - 1.f));
			Vertex[iIndex].vNormal = D3DXVECTOR3(0.f, 0.f, 0.f);
		}
	}
	
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

			D3DXVECTOR3		vDest, vSour, vNormal;
			vDest = Vertex[Index[iTriCnt]._2].vPos - Vertex[Index[iTriCnt]._1].vPos;
			vSour = Vertex[Index[iTriCnt]._3].vPos - Vertex[Index[iTriCnt]._2].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);

			Vertex[Index[iTriCnt]._1].vNormal += vNormal;
			Vertex[Index[iTriCnt]._2].vNormal += vNormal;
			Vertex[Index[iTriCnt]._3].vNormal += vNormal;

			++iTriCnt;

			Index[iTriCnt]._1 = iIndex + iCountX;
			Index[iTriCnt]._2 = iIndex + 1;
			Index[iTriCnt]._3 = iIndex;

			vDest = Vertex[Index[iTriCnt]._2].vPos - Vertex[Index[iTriCnt]._1].vPos;
			vSour = Vertex[Index[iTriCnt]._3].vPos - Vertex[Index[iTriCnt]._2].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);

			Vertex[Index[iTriCnt]._1].vNormal += vNormal;
			Vertex[Index[iTriCnt]._2].vNormal += vNormal;
			Vertex[Index[iTriCnt]._3].vNormal += vNormal;

			++iTriCnt;
		}
	}

	for (unsigned long i = 0; i < iVertexCount; ++i)
	{
		D3DXVec3Normalize(&Vertex[i].vNormal, &Vertex[i].vNormal);		
	}
	
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VTXTEX) * iVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData;
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = Vertex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);


	//::Safe_Delete_Array(Vertex);

	if (FAILED(hr))
		return E_FAIL;


	
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

	//if (FAILED(hr))
	//{
	//	MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
	//	return hr;

	//}

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
	::Safe_Delete_Array(Vertex);
	return 0;
}

CResources * CRcTerrain::CloneResource(void)
{
	CResources* pResource = this;

	pResource->AddRef();

	return pResource;
}

DWORD * CRcTerrain::LoadImage(void)
{
	D3DXIMAGE_INFO		ImgInfo;

	if (FAILED(D3DXGetImageInfoFromFile(L"../Resource/Terrain/Stage1Height.png", &ImgInfo)))
	{
		MessageBox(NULL, L"Can't not open the texture file.", L"Error", MB_OK);
	}

	if (FAILED(D3DXCreateTextureFromFileEx(CParsingDevice9::GetInstance()->m_pDevice,
		L"../Resource/Terrain/Stage1Height.png", ImgInfo.Width, ImgInfo.Height,
		ImgInfo.MipLevels, 0,
		ImgInfo.Format, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0,
		&ImgInfo, NULL, &m_pTexHeightMap)))
	{
		MessageBox(NULL, L"Can't not open the texture file.", L"Error", MB_OK);
	}


	return NULL;
}
