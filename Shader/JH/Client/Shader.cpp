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

	hr = D3DX11CompileFromFile(
		wstrFilePath.c_str(), // 셰이더 소스 코드를 담고 있는.fx파일의 이름
		NULL, 
		NULL,
		wstrShaderName, // 셰이더 프로그램의 진입점, 셰이더 함수의 이름
		wstrShaderVersion, //사용할 셰이더 버전(Directx 11의 경우 ps_5_0을 사용)
		dwShaderFlags, // 셰이더 코드의 컴파일 방식에 영향을 미치는 플래그
		0, 
		NULL,
		&pShaderBlob, // 컴파일된 셰이더를 담은 ID3DBlob구조체를 가리키는 포인터를 돌려줌
		&pErrorBlob, // 컴파일 오류시 오류 메시지를 담은 문자열을 담은 ID3DBlob구조체를 가리키는 포인터를 돌려줌
		NULL); // 비동기 컴파일시 오류코드를 조회하는데 쓰임 pPump에 널이라면 이매개변수도 널

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL) // pErrorBlob오류 메시지가 있을경우
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
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			
		};

		UINT numElements = ARRAYSIZE(layout);

		// D3D11_INPUT_ELEMENT_DESC와 vertex shader semantic의 매칭이 이루어짐
		hr = CDevice::GetInstance()->m_pDevice->CreateInputLayout( // 입력배치 생성하는 함수
			layout, // 정점 구조체를 서술하는 D3D11_INPUT_ELEMENT_DESC들의 배열
			numElements, // D3D11_INPUT_ELEMENT_DESC배열의 원소개수
			pShaderBlob->GetBufferPointer(), // 정점셰이더를 컴파일해서 얻은 바이트 코드를 가리키는 포인터
			pShaderBlob->GetBufferSize(), // 바이트 코드의 크기
			&m_pVertexLayout); // 생성된 입력배치를 이 포인터를 통해서 돌려줌

		pShaderBlob->Release();

		if (FAILED(hr))
			return hr;

		CDevice::GetInstance()->m_pDeviceContext->IASetInputLayout(m_pVertexLayout); 
	}

	else if (_SType == SHADER_PS)
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
	CShader*      pShader = new CShader;
	if (FAILED(pShader->Ready_ShaderFile(wstrFilePath, wstrShaderName, wstrShaderVersion, _SType)))
		Safe_Delete(pShader);

	return pShader;
}

CShader * CShader::CloneShader(void)
{
	return new CShader(*this);
}