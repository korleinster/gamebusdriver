#include "stdafx.h"
#include "CubeCol.h"
#include "Device.h"


CCubeCol::CCubeCol()
{
}


CCubeCol::~CCubeCol()
{
}

HRESULT CCubeCol::CreateBuffer(void)
{
	//여기
	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXCOL);
	m_iVertexOffsets = 0;

	D3DXVECTOR3 vMin(-1.f, -1.f, -1.f);
	D3DXVECTOR3 vMax(1.f, 1.f, 1.f);


	VTXCOL Vertex[] =
	{
		{ D3DXVECTOR3(vMin.x, vMax.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMax.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMin.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMin.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMax.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMax.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMin.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMin.y, vMax.z), D3DXCOLOR(1,0,0,1) },
	};

	D3D11_BUFFER_DESC vbd; // 버퍼 디스크 = 버퍼의 용도와 크기와 접근권한
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT; // 버퍼는 연산을 위해 필요한 것인대 하드웨어에서 연산이 가능한 객체는 2개가있음 CPU, GPU
	vbd.ByteWidth = sizeof(VTXCOL) * 8; // 크기
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 용도 = 버퍼는 3개가 있음. 버텍스, 인덱스, 콘스턴트 
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData; // 서브리소스는 데이터인대 = 실질적인 값을 의미함
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = Vertex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	UINT Index[] =
	{
		//+x
		1, 5, 6,
		1, 6, 2,
		//-x
		4, 0, 3,
		4, 3, 7,
		//+y
		4, 5, 1,
		4, 3, 7,
		//-y
		3, 2, 6,
		3, 6, 7,
		//+z
		7, 6, 5,
		7, 5, 4,
		//-z
		0, 1, 2,
		0, 2, 3,
	};

	m_iIndex = 36;

	D3D11_BUFFER_DESC ibd; // 권한/크기/용도
	ibd.Usage = D3D11_USAGE_DEFAULT; // 권한
	ibd.ByteWidth = sizeof(UINT) * m_iIndex; // 크기
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // 용도
	ibd.CPUAccessFlags = 0; // CPU접근권한 설정
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData; // 실제 데이터
	iinitData.pSysMem = Index;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	//다이렉트9 != 다이렉트11
	//다이렉트9 = 컨스턴트 테이블이 존재 수 많은 글로벌 변수가 값은 채워지지 않고 할당만 되어있었음 = 메모리 낭비
	//다이렉트11 = 상수는 약속을함 상수 버퍼가 채워지지 않으면 셰이더가 작동하지않음 = 메모리 최적화

	D3D11_BUFFER_DESC cbd; // 접근권한 / 용도/ 크기
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);

	/*D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC; // CPU에게 읽기를 할수있는 권한을 주심
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 라이트의 권한을 주었지 ㅇㅋ?
	CDevice::GetInstance()->m_pDevice->CreateBuffer(&bd, NULL, &m_ConstantBuffer);*/


	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;

	}

	return S_OK;
}

CCubeCol * CCubeCol::Create(void)
{
	CCubeCol*	pCubeCol = new CCubeCol;

	if (FAILED(pCubeCol->CreateBuffer()))
		Safe_Delete(pCubeCol);

	return pCubeCol;
}

CResources * CCubeCol::CloneResource(void)
{
	CResources* pResource = new CCubeCol(*this);

	pResource->AddRef();

	return pResource;
}

