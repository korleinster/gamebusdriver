#include"stdafx.h"
#include"RenderTarget.h"
#include "Device.h"
#include "ResourcesMgr.h"
#include "VIBuffer.h"
#include "ShaderMgr.h"
#include "Shader.h"

#pragma pack(push,1)
struct CB_VS
{
	D3DXMATRIX m_mWorld;
};
#pragma pack(pop)

CRenderTarget::CRenderTarget()
	:m_pTargetRT(nullptr)
	, m_pTagetRTV(nullptr)
	, m_pTagetSRV(nullptr)
	, m_pDebugBuffer(nullptr)
	, m_pDebugBufferVS(nullptr)
	, m_pDebugBufferPS(nullptr)
	, m_pSamplerState(nullptr)
	, m_pConstantBuffer(nullptr)
{

}

CRenderTarget::~CRenderTarget()
{

}

HRESULT CRenderTarget::Ready_RenderTarget(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format)
{
	// Allocate the depth stencil target
	D3D11_TEXTURE2D_DESC dtd = {
		sizeX, //UINT Width;
		sizeY, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		format, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, nullptr, &m_pTargetRT);

	// Create the render target views

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		format,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_pTargetRT, &rtsvd, &m_pTagetRTV);

	// Create the resource views
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd =
	{
		format,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	dsrvd.Texture2D.MipLevels = 1;

	CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_pTargetRT, &dsrvd, &m_pTagetSRV);

	return S_OK;
}

void CRenderTarget::Ready_DebugBuffer(const float& posX, const float& posY, const float& sizeX, const float& sizeY, const  UINT& type)
{
	//Clone DebugBuffer
	m_pDebugBuffer = dynamic_cast<CVIBuffer*>(CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex"));

	//Create Sampler State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CDevice::GetInstance()->m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerState);

	//Create Constant Buffer
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(CB_VS);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_pConstantBuffer);

	//Calculate World Matrix
	D3DXMATRIX matScale, matTransform;
	D3DXMatrixIdentity(&m_matWorld);

	D3DXMatrixScaling(&matScale, sizeX, sizeY, 1.f);
	D3DXMatrixTranslation(&matTransform, posX, posY, 0.f);

	m_matWorld = matScale * matTransform;

	//Clone Shader
	m_pDebugBufferVS = CShaderMgr::GetInstance()->Clone_Shader(L"DebugBufferVS");
	if (type == 0)
		m_pDebugBufferPS = CShaderMgr::GetInstance()->Clone_Shader(L"DebugBufferPS");
	else if (type == 1)
		m_pDebugBufferPS = CShaderMgr::GetInstance()->Clone_Shader(L"DebugBufferPS_Red");

}

void CRenderTarget::RenderRT(ID3D11DeviceContext* pd3dImmediateContext)
{

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_VS* pVSPerObject = (CB_VS*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSPerObject->m_mWorld, &m_matWorld);
	pd3dImmediateContext->Unmap(m_pConstantBuffer, 0);

	pd3dImmediateContext->IASetInputLayout(m_pDebugBufferVS->m_pVertexLayout);

	pd3dImmediateContext->VSSetShader(m_pDebugBufferVS->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	pd3dImmediateContext->PSSetShader(m_pDebugBufferPS->m_pPixelShader, NULL, 0);
	pd3dImmediateContext->PSSetShaderResources(0, 1, &m_pTagetSRV);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	m_pDebugBuffer->Render();
}

void CRenderTarget::Release()
{
	Safe_Release(m_pTargetRT);
	Safe_Release(m_pTagetRTV);
	Safe_Release(m_pTagetSRV);

	Safe_Delete(m_pDebugBuffer);
	Safe_Delete(m_pDebugBufferVS);
	Safe_Delete(m_pDebugBufferPS);

	Safe_Release(m_pSamplerState);
	Safe_Release(m_pConstantBuffer);
}

CRenderTarget* CRenderTarget::Create(const  UINT& sizeX, const  UINT& sizeY, DXGI_FORMAT format)
{
	CRenderTarget*	pInstance = new CRenderTarget();

	if (FAILED(pInstance->Ready_RenderTarget(sizeX, sizeY, format)))
	{
		MessageBox(NULL, L"System Message", L"RenderTarget Created Failed", MB_OK);
		Safe_Delete(pInstance);
	}

	return pInstance;
}