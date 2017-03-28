#include "stdafx.h"
#include "MultiRenderTarget.h"
#include "Device.h"
#include "Camera.h"

#pragma pack(push,1)
struct CB_GBUFFER_UNPACK
{
	D3DXVECTOR4 PerspectiveValues;
	D3DXMATRIX  ViewInv;
};
#pragma pack(pop)

CMultiRenderTarget::CMultiRenderTarget()
: m_pGBufferUnpackCB(NULL), 
m_DepthStencilRT(NULL), m_ColorSpecIntensityRT(NULL), m_NormalRT(NULL), m_SpecPowerRT(NULL),
m_DepthStencilDSV(NULL), m_DepthStencilReadOnlyDSV(NULL), m_ColorSpecIntensityRTV(NULL), m_NormalRTV(NULL), m_SpecPowerRTV(NULL),
m_DepthStencilSRV(NULL), m_ColorSpecIntensitySRV(NULL), m_NormalSRV(NULL), m_SpecPowerSRV(NULL),
m_SobelRT(NULL), m_SobelRTV(NULL), m_SobelSRV(NULL),
m_DepthStencilState(NULL)
{

}

CMultiRenderTarget::~CMultiRenderTarget()
{
	Release();
}

HRESULT CMultiRenderTarget::Initialize(UINT width, UINT height)
{
	Release();

	// Texture formats
	static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
	static const DXGI_FORMAT basicColorTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalTextureFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	static const DXGI_FORMAT specPowTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT sobelTextureFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// Render view formats
	static const DXGI_FORMAT depthStencilRenderViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const DXGI_FORMAT basicColorRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalRenderViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	static const DXGI_FORMAT specPowRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT sobelRenderViewFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// Resource view formats
	static const DXGI_FORMAT depthStencilResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	static const DXGI_FORMAT basicColorResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalResourceViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	static const DXGI_FORMAT specPowResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT sobelResourceViewFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

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
	//m_DepthStencilRT->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Depth Stencil"), "GBuffer - Depth Stencil");

	// Allocate the base color with specular intensity target
	dtd.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	dtd.Format = basicColorTextureFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_ColorSpecIntensityRT));
	//m_ColorSpecIntensityRT->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Base Color Specular Intensity"), "GBuffer - Base Color Specular Intensity");

	// Allocate the base color with specular intensity target
	dtd.Format = normalTextureFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_NormalRT));
	//m_NormalRT->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Normal"), "GBuffer - Normal");

	// Allocate the specular power target
	dtd.Format = specPowTextureFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_SpecPowerRT));
	//m_SpecPowerRT->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Specular Power"), "GBuffer - Specular Power");

	// Allocate the Sobel target
	dtd.Format = sobelTextureFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_SobelRT));
	//m_SobelRT->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Sobel"), "GBuffer - Sobel");

	// Create the render target views
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd =
	{
		depthStencilRenderViewFormat,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilView(m_DepthStencilRT, &dsvd, &m_DepthStencilDSV));
	//m_DepthStencilDSV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Depth Stencil DSV"), "GBuffer - Depth Stencil DSV");

	dsvd.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilView(m_DepthStencilRT, &dsvd, &m_DepthStencilReadOnlyDSV));
	//m_DepthStencilReadOnlyDSV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Depth Stencil Read Only DSV"), "GBuffer - Depth Stencil Read Only DSV");

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		basicColorRenderViewFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_ColorSpecIntensityRT, &rtsvd, &m_ColorSpecIntensityRTV));
	//_ColorSpecIntensityRTV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Color Spec Intensity RTV"), "GBuffer - Color Spec Intensity RTV");

	rtsvd.Format = normalRenderViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_NormalRT, &rtsvd, &m_NormalRTV));
	//m_NormalRTV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Normal RTV"), "GBuffer - Normal RTV");

	rtsvd.Format = specPowRenderViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_SpecPowerRT, &rtsvd, &m_SpecPowerRTV));
	//m_SpecPowerRTV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Spec Power RTV"), "GBuffer - Spec Power RTV");

	rtsvd.Format = sobelRenderViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRenderTargetView(m_SobelRT, &rtsvd, &m_SobelRTV));
	//m_SobelRTV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Sobel RTV"), "GBuffer - Sobel RTV");

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
	//m_DepthStencilSRV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Depth SRV"), "GBuffer - Depth SRV");

	dsrvd.Format = basicColorResourceViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_ColorSpecIntensityRT, &dsrvd, &m_ColorSpecIntensitySRV));
	//m_ColorSpecIntensitySRV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Color Spec Intensity SRV"), "GBuffer - Color Spec Intensity SRV");

	dsrvd.Format = normalResourceViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_NormalRT, &dsrvd, &m_NormalSRV));
	//m_NormalSRV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Normal SRV"), "GBuffer - Normal SRV");

	dsrvd.Format = specPowResourceViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_SpecPowerRT, &dsrvd, &m_SpecPowerSRV));
	//m_SpecPowerSRV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Spec Power SRV"), "GBuffer - Spec Power SRV");

	dsrvd.Format = sobelResourceViewFormat;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_SobelRT, &dsrvd, &m_SobelSRV));
	//m_SobelSRV->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Sobel SRV"), "GBuffer - Sobel SRV");

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
	//m_DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBuffer - Depth Stencil Mark DS"), "GBuffer - Depth Stencil Mark DS");

	// Create constant buffers
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_GBUFFER_UNPACK);
	// constant buffer용 버퍼 생성
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pGBufferUnpackCB));
	//m_pGBufferUnpackCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("GBufferUnpack CB"), "GBufferUnpack CB");

	return S_OK;
}

void CMultiRenderTarget::Release()
{
	Safe_Release(m_pGBufferUnpackCB);

	// Clear all allocated targets
	Safe_Release(m_DepthStencilRT);
	Safe_Release(m_ColorSpecIntensityRT);
	Safe_Release(m_NormalRT);
	Safe_Release(m_SpecPowerRT);
	Safe_Release(m_SobelRT);

	// Clear all views
	Safe_Release(m_DepthStencilDSV);
	Safe_Release(m_DepthStencilReadOnlyDSV);
	Safe_Release(m_ColorSpecIntensityRTV);
	Safe_Release(m_NormalRTV);
	Safe_Release(m_SpecPowerRTV);
	Safe_Release(m_SobelRTV);
	Safe_Release(m_DepthStencilSRV);
	Safe_Release(m_ColorSpecIntensitySRV);
	Safe_Release(m_NormalSRV);
	Safe_Release(m_SpecPowerSRV);
	Safe_Release(m_SobelSRV);

	// Clear the depth stencil state
	Safe_Release(m_DepthStencilState);
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
	pd3dImmediateContext->ClearRenderTargetView(m_ColorSpecIntensityRTV, ClearColor);
	pd3dImmediateContext->ClearRenderTargetView(m_NormalRTV, ClearColor);
	pd3dImmediateContext->ClearRenderTargetView(m_SpecPowerRTV, ClearColor);
	pd3dImmediateContext->ClearRenderTargetView(m_SobelRTV, ClearColor);

	// Bind all the render targets togther
	ID3D11RenderTargetView* rt[4] = { m_ColorSpecIntensityRTV, m_NormalRTV, m_SpecPowerRTV, m_SobelRTV };

	pd3dImmediateContext->OMSetRenderTargets(
		4, // 렌더타겟의 수
		rt, // 렌더타겟 뷰의 배열
		m_DepthStencilDSV);

	pd3dImmediateContext->OMSetDepthStencilState(m_DepthStencilState, 1);
}

void CMultiRenderTarget::End_MRT(ID3D11DeviceContext* pd3dImmediateContext)
{
	// 삼각형을 렌더링할때 필요한 버텍스들을 gpu가 가져오는 방식을 정의한다
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Little cleanup
	ID3D11RenderTargetView* rt[4] = { NULL, NULL, NULL, NULL };
	pd3dImmediateContext->OMSetRenderTargets(
		4, // 렌더타겟의 수
		rt, // 렌더타겟 뷰의 배열
		m_DepthStencilReadOnlyDSV);
}

void CMultiRenderTarget::PrepareForUnpack(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Fill the GBuffer unpack constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// 상수 버퍼의 내용을 쓸 수 있도록 잠금
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pGBufferUnpackCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource),);
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