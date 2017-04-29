#include "stdafx.h"
#include "MultiRenderTarget.h"
#include "Device.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "ResourcesMgr.h"
#include "VIBuffer.h"

#pragma pack(push,1)
struct CB_GBUFFER_UNPACK
{
	D3DXVECTOR4 PerspectiveValues;
	D3DXMATRIX  ViewInv;
};
#pragma pack(pop)
#pragma pack(push,1)
struct CB_VS
{
	D3DXMATRIX m_mWorld;
};
#pragma pack(pop)

CMultiRenderTarget::CMultiRenderTarget()
	: m_pCB(NULL),
	m_DepthStencilRT(NULL),// m_ColorSpecIntensityRT(NULL), m_NormalRT(NULL), m_SpecPowerRT(NULL),
	m_DepthStencilDSV(NULL),// m_DepthStencilReadOnlyDSV(NULL), m_ColorSpecIntensityRTV(NULL), m_NormalRTV(NULL), m_SpecPowerRTV(NULL),
	m_DepthStencilSRV(NULL), // m_ColorSpecIntensitySRV(NULL), m_NormalSRV(NULL), m_SpecPowerSRV(NULL),
	m_DepthStencilState(NULL),
	m_pGBufferVisVertexShader(NULL),
	m_pGBufferVisPixelShader(NULL),
	m_pDepthDebugBuffer(NULL)
{

}

CMultiRenderTarget::~CMultiRenderTarget()
{
	Release();
}

HRESULT CMultiRenderTarget::Initialize(UINT width, UINT height)
{

	// Allocate the depth stencil target
	D3D11_TEXTURE2D_DESC dtd =
	{
		width, // WINCX
		height, //WINCY
		1, // MipLevels;
		1, // ArraySize;
		DXGI_FORMAT_UNKNOWN, // Format;
		1, // SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,// BindFlags;
		0,// CPUAccessFlags;
		0// MiscFlags;    
	};

	// 텍스쳐 생성
	dtd.Format = depthStencilTextureFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_DepthStencilRT));

	// Create the render target views
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd =
	{
		depthStencilRenderViewFormat,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilView(m_DepthStencilRT, &dsvd, &m_DepthStencilDSV));

	dsvd.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilView(m_DepthStencilRT, &dsvd, &m_DepthStencilReadOnlyDSV));

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		basicColorRenderViewFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	// Create the resource views
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd =
	{
		depthStencilResourceViewFormat,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	dsrvd.Texture2D.MipLevels = 1;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_DepthStencilRT, &dsrvd, &m_DepthStencilSRV));

	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_DepthStencilState));

	// Create constant buffers
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_VS);
	// constant buffer용 버퍼 생성
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCB));
	cbDesc.ByteWidth = sizeof(CB_GBUFFER_UNPACK);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pGBufferUnpackCB));

	m_pGBufferVisVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"DebugBufferVS");
	m_pGBufferVisPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"DebugBufferPS_Red");

	//Clone DebugBuffer
	m_pDepthDebugBuffer = dynamic_cast<CVIBuffer*>(CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex"));

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

	//Calculate World Matrix
	D3DXMATRIX matScale, matTransform;
	D3DXMatrixIdentity(&m_matDepthWorld);

	D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 1.f);
	D3DXMatrixTranslation(&matTransform, -0.8f, 0.8f, 0.f);

	m_matDepthWorld = matScale * matTransform;

	return S_OK;
}

void CMultiRenderTarget::SetRT(CRenderTarget* pRT)
{
	m_vecRT.push_back(pRT);
}

void CMultiRenderTarget::RenderMRT(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->RSSetState(NULL);

	int rtSize = m_vecRT.size();
	for (UINT i = 0; i < rtSize; ++i)
	{
		m_vecRT[i]->RenderRT(pd3dImmediateContext);
	}
}

void CMultiRenderTarget::RenderDepth(ID3D11DeviceContext* pd3dImmediateContext)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_VS* pVSPerObject = (CB_VS*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSPerObject->m_mWorld, &m_matDepthWorld);
	pd3dImmediateContext->Unmap(m_pCB, 0);

	pd3dImmediateContext->IASetInputLayout(m_pGBufferVisVertexShader->m_pVertexLayout);

	pd3dImmediateContext->VSSetShader(m_pGBufferVisVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &m_pCB);

	pd3dImmediateContext->PSSetShader(m_pGBufferVisPixelShader->m_pPixelShader, NULL, 0);
	pd3dImmediateContext->PSSetShaderResources(0, 1, &m_DepthStencilSRV);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	m_pDepthDebugBuffer->Render();
}



void CMultiRenderTarget::Release()
{
	Safe_Release(m_pCB);

	// Clear all allocated targets
	Safe_Release(m_DepthStencilRT);

	// Clear all views
	Safe_Release(m_DepthStencilDSV);
	Safe_Release(m_DepthStencilReadOnlyDSV);

	// Clear the depth stencil state
	Safe_Release(m_DepthStencilState);

	Safe_Delete(m_pGBufferVisVertexShader);
	Safe_Delete(m_pGBufferVisPixelShader);

	for (int i = 0; i < m_vecRT.size(); ++i)
	{
		Safe_Delete(m_vecRT[i]);
	}
}

void CMultiRenderTarget::Begin_MRT(ID3D11DeviceContext* pd3dImmediateContext)
{
	// 깊이 스텐실 버퍼 클리어
	pd3dImmediateContext->ClearDepthStencilView(
		m_DepthStencilDSV, // 지우고자 하는 부분
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, // 깊이버퍼, 스텐실버퍼 지우기 
		1.0f,  // 깊이 버퍼의 모든 픽셀에 설정할 값
		0); // 스텐실 버퍼 모든 픽셀에 설정할 값

			// You only need to do this if your scene doesn't cover the whole visible area
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	// 각각의 렌더타겟뷰 클리어
	int rtSize = m_vecRT.size();
	for (UINT i = 0; i < rtSize; ++i)
	{
		pd3dImmediateContext->ClearRenderTargetView(m_vecRT[i]->GetRTV(), ClearColor);
	}

	// Bind all the render targets togther
	ID3D11RenderTargetView** rt = new ID3D11RenderTargetView*[rtSize];
	for (UINT i = 0; i < rtSize; ++i)
	{
		rt[i] = m_vecRT[i]->GetRTV();
	}

	pd3dImmediateContext->OMSetRenderTargets(
		rtSize, // 렌더타겟의 수
		rt, // 렌더타겟 뷰의 배열
		m_DepthStencilDSV);

	pd3dImmediateContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	delete[] rt;
}

void CMultiRenderTarget::End_MRT(ID3D11DeviceContext* pd3dImmediateContext)
{
	// 삼각형을 렌더링할때 필요한 버텍스들을 gpu가 가져오는 방식을 정의한다
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Little cleanup
	//ID3D11RenderTargetView* rt[4] = { NULL, NULL, NULL, NULL };

	int rtSize = m_vecRT.size();
	ID3D11RenderTargetView** rt = new ID3D11RenderTargetView*[rtSize];
	for (UINT i = 0; i < rtSize; ++i)
	{
		rt[i] = NULL;
	}

	pd3dImmediateContext->OMSetRenderTargets(
		rtSize, // 렌더타겟의 수
		rt, // 렌더타겟 뷰의 배열
		m_DepthStencilReadOnlyDSV);

	delete[] rt;
}

void CMultiRenderTarget::PrepareForUnpack(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Fill the GBuffer unpack constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// 상수 버퍼의 내용을 쓸 수 있도록 잠금
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pGBufferUnpackCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	// 상수 버퍼의 데이터에 대한 포인터를 가져옴
	CB_GBUFFER_UNPACK* pGBufferUnpackCB = (CB_GBUFFER_UNPACK*)MappedResource.pData;
	const D3DXMATRIX* pProj = CCamera::GetInstance()->GetProjMatrix();

	// 상수 버퍼에 값을 복사
	pGBufferUnpackCB->PerspectiveValues.x = 1.0f / pProj->m[0][0];
	pGBufferUnpackCB->PerspectiveValues.y = 1.0f / pProj->m[1][1];
	pGBufferUnpackCB->PerspectiveValues.z = pProj->m[3][2];
	pGBufferUnpackCB->PerspectiveValues.w = -pProj->m[2][2];

	D3DXMATRIX matViewInv;
	D3DXMatrixInverse(&matViewInv, NULL, CCamera::GetInstance()->GetViewMatrix());
	D3DXMatrixTranspose(&pGBufferUnpackCB->ViewInv, &matViewInv);
	// 상수 버퍼의 잠금을 품
	pd3dImmediateContext->Unmap(m_pGBufferUnpackCB, 0);
	// 픽셀 셰이더의 상수 버퍼를 바뀐 값으로 바꿈
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &m_pGBufferUnpackCB);
}