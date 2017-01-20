#include "stdafx.h"
#include "Shader.h"
#include "Device.h"

CShader::CShader()
{
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pVertexLayout = NULL;
	m_pHullShader = NULL;
	m_pDomainShader = NULL;
	m_dwRefCount = 1;
}

CShader::CShader(const CShader & rhs)
{
	m_pVertexShader = rhs.m_pVertexShader;
	m_pVertexLayout = rhs.m_pVertexLayout;
	m_pPixelShader = rhs.m_pPixelShader;
	m_pHullShader = rhs.m_pHullShader;
	m_pDomainShader = rhs.m_pDomainShader;
	m_dwRefCount = rhs.m_dwRefCount;
	++m_dwRefCount;
}


CShader::~CShader()
{
	Release();
}

HRESULT CShader::Ready_ShaderFile(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* pErrorBlob = NULL;
	ID3DBlob* pShaderBlob = NULL;
	
	// hlsl 컴파일
	hr = D3DX11CompileFromFile(
		wstrFilePath.c_str(), // 셰이더 파일 경로
		NULL,
		NULL, 
		wstrShaderName, // 셰이더 실행이 시작되는 진입 점 함수의 이름
		wstrShaderVersion, // 셰이더 버전
		dwShaderFlags,
		0,
		NULL,
		&pShaderBlob,
		&pErrorBlob,
		NULL);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	if (_SType == SHADER_VS)
	{
		// 정점 셰이더 객체를 만듬
		hr = CDevice::GetInstance()->m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

		if (FAILED(hr))
		{
			pShaderBlob->Release();
			return E_FAIL;
		}

		// D3D11_INPUT_ELEMENT_DESC는 다음의 필드를 가짐
		// SemanticName - 시맨틱이름은 이 요소의 본질 또는 목적을 설명하는 단어 ex)버텍스위치를 위한 좋은 시맨틱이름은 POSITION
		// SemanticIndex - 시맨틱인덱스는 시맨틱이름을 보충한다, 동일한 본질을 여러개 가질경우 시맨틱이름은 같지만 인덱스가 다르다
		// Format - 잉 요소에 사용하는 데이타 타입을 정의한다
		// InputSlot - GPU에게 이요소를 어떤 버텍스 버퍼로부터 가져와야 하는지를 말함
		// AlignedByteOffset - GPU에게 이요소가 메모리덩어리 시작지점부터 얼마나 떨어진 위치에 있는가를 알려준다
		// InputSlotClass - 인스턴싱을 위해 설정 여기선 디폴트
		// InstanceDataStepRate - 인스턴싱을 위해 설정 여기선 디폴트
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			/*{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },*/
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};

		UINT numElements = ARRAYSIZE(layout);
		// 정점 입력 레이아웃을 생성
		hr = CDevice::GetInstance()->m_pDevice->CreateInputLayout(layout, numElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pVertexLayout);

		CDevice::GetInstance()->m_pDeviceContext->IASetInputLayout(m_pVertexLayout);
	}
	else if (_SType == SHADER_PS)
	{
		hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
	}
	else if (_SType == SHADER_HS)
	{
		hr = CDevice::GetInstance()->m_pDevice->CreateHullShader(pShaderBlob->GetBufferPointer(),pShaderBlob->GetBufferSize(), NULL, &m_pHullShader);
	}
	else if (_SType == SHADER_DS)
	{
		hr = CDevice::GetInstance()->m_pDevice->CreateDomainShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDomainShader);
	}

	// 버퍼들은 더이상 사용되지 않으므로 할당을 해제
	pShaderBlob->Release();
	
	if (FAILED(hr))
		return hr;

	return S_OK;
}

DWORD CShader::Release(void)
{
	if (m_dwRefCount == 1)
	{
		::Safe_Release(m_pVertexShader);
		::Safe_Release(m_pPixelShader);
		::Safe_Release(m_pVertexLayout);
	}

	else
		--m_dwRefCount;

	return m_dwRefCount;
}

CShader * CShader::Create(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType)
{
	CShader*		pShader = new CShader;
	if (FAILED(pShader->Ready_ShaderFile(wstrFilePath, wstrShaderName, wstrShaderVersion, _SType)))
		Safe_Delete(pShader);

	return pShader;
}

CShader * CShader::CloneShader(void)
{
	return new CShader(*this);
}
