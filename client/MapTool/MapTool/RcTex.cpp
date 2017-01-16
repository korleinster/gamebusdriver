#include "stdafx.h"
#include "RcTex.h"
#include "Device.h"


CRcTex::CRcTex()
{
}

CRcTex::CRcTex(CRcTex * rhs)
{

}

CRcTex::~CRcTex()
{
}

HRESULT CRcTex::CreateBuffer(void)
{
	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXTEX);
	m_iVertexOffsets = 0;



	VTXTEX Vertex[] =
	{
		/*{ D3DXVECTOR3(-1.f, 0.f, 1.f), D3DXVECTOR2(0.f, 0.f) },
		{ D3DXVECTOR3(1.f, 0.f, 1.f),  D3DXVECTOR2(1.f, 0.f) },
		{ D3DXVECTOR3(1.f, 0.f, -1.f), D3DXVECTOR2(1.f, 1.f) },
		{ D3DXVECTOR3(-1.f, 0.f, -1.f), D3DXVECTOR2(0.f, 1.f) }*/

		{ D3DXVECTOR3(-1.f, 1.f, 0.f),D3DXVECTOR3(0.f, 1.f, 0.f) , D3DXVECTOR2(0.f, 0.f) },
		{ D3DXVECTOR3(1.f, 1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f, 0.f) },
		{ D3DXVECTOR3(1.f, -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(1.f, 1.f) },
		{ D3DXVECTOR3(-1.f, -1.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), D3DXVECTOR2(0.f, 1.f) }
	};

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VTXTEX) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData;
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = Vertex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);

	if (FAILED(hr))
		return E_FAIL;

	UINT Index[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	m_iIndex = 6;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = Index;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

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

	return S_OK;
}

CRcTex * CRcTex::Create(void)
{
	CRcTex* pRcTex = new CRcTex;

	if (FAILED(pRcTex->CreateBuffer()))
	{
		::Safe_Delete(pRcTex);
	}
	return pRcTex;
}

CResources * CRcTex::CloneResource(void)
{
	return nullptr;
}
