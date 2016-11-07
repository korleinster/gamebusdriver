#include "stdafx.h"
#include "Shader.h"
#include "Device.h"


CShader::CShader()
{
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pVertexLayout = NULL;
	m_dwRefCount = 1;
}

CShader::CShader(const CShader & rhs)
{
	m_pVertexShader = rhs.m_pVertexShader;
	m_pVertexLayout = rhs.m_pVertexLayout;
	m_pPixelShader = rhs.m_pPixelShader;
	m_dwRefCount = rhs.m_dwRefCount;
	++m_dwRefCount;
}


CShader::~CShader()
{
	Release();
}

HRESULT CShader::Ready_ShaderFile(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType)
{
	//쉐이더는 gpu를 활용하는 기법.

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
	
	hr = D3DX11CompileFromFile(wstrFilePath.c_str(), NULL, NULL, wstrShaderName, wstrShaderVersion, dwShaderFlags, 0, NULL, &pShaderBlob, &pErrorBlob, NULL);
	//셰이더코드 불러오는 함수

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

		hr = CDevice::GetInstance()->m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

		if (FAILED(hr))
		{
			pShaderBlob->Release();
			return E_FAIL;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};//시멘틱 라벨

		UINT numElements = ARRAYSIZE(layout);// 할당

		hr = CDevice::GetInstance()->m_pDevice->CreateInputLayout(layout, numElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pVertexLayout);
		pShaderBlob->Release();
		if (FAILED(hr))
			return hr;

		CDevice::GetInstance()->m_pDeviceContext->IASetInputLayout(m_pVertexLayout);
		//셰이더가 안되면 여기확인
	}

	else if (_SType == SHADER_PS)//픽셸쉐이더 할당
	{
		hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
		pShaderBlob->Release();

		if (FAILED(hr))
			return hr;
	}

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
