#include "stdafx.h"
#include "LightMgr.h"
#include "Device.h"
#include "Camera.h"
#include "TargetMgr.h"
#include "MultiRenderTarget.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "ShaderMgr.h"

IMPLEMENT_SINGLETON(CLightMgr)

const D3DXVECTOR3 GammaToLinear(const D3DXVECTOR3& color)
{
	return D3DXVECTOR3(color.x * color.x, color.y * color.y, color.z * color.z);
}

#pragma pack(push,1)
struct CB_DIRECTIONAL
{
	D3DXVECTOR3 vAmbientLower;
	float pad;
	D3DXVECTOR3 vAmbientRange;
	float pad2;
	D3DXVECTOR3 vDirToLight;
	float pad3;
	D3DXVECTOR3 vDirectionalColor;
	float pad4;
	D3DXMATRIX ToShadowSpace;
	D3DXVECTOR4 ToCascadeSpace[3];
};

struct CB_POINT_LIGHT_DOMAIN
{
	D3DXMATRIX WolrdViewProj;
};

struct CB_POINT_LIGHT_PIXEL
{
	D3DXVECTOR3 PointLightPos;
	float PointLightRangeRcp;
	D3DXVECTOR3 PointColor;
	float pad;
	D3DXVECTOR2 LightPerspectiveValues;
	float pad2[2];
};

struct CB_SPOT_LIGHT_DOMAIN
{
	D3DXMATRIX WolrdViewProj;
	float fSinAngle;
	float fCosAngle;
	float pad[2];
};

struct CB_SPOT_LIGHT_PIXEL
{
	D3DXVECTOR3 SpotLightPos;
	float SpotLightRangeRcp;
	D3DXVECTOR3 vDirToLight;
	float SpotCosOuterCone;
	D3DXVECTOR3 SpotColor;
	float SpotCosConeAttRange;
};

struct CB_CAPSULE_LIGHT_DOMAIN
{
	D3DXMATRIX WolrdViewProj;
	float HalfCapsuleLen;
	float CapsuleRange;
	float pad[2];
};

struct CB_CAPSULE_LIGHT_PIXEL
{
	D3DXVECTOR3 CapsuleLightPos;
	float CapsuleLightRangeRcp;
	D3DXVECTOR3 CapsuleDir;
	float CapsuleLen;
	D3DXVECTOR3 CapsuleColor;
	float pad;
};
#pragma pack(pop)

CLightMgr::CLightMgr()
	:m_bShowLightVolume(false),
	m_pDirLightVertexShader(NULL), m_pDirLightPixelShader(NULL), m_pDirLightCB(NULL),
	m_pPointLightVertexShader(NULL), m_pPointLightHullShader(NULL), m_pPointLightDomainShader(NULL), m_pPointLightPixelShader(NULL),
	m_pPointLightDomainCB(NULL), m_pPointLightPixelCB(NULL),
	m_pSpotLightVertexShader(NULL), m_pSpotLightHullShader(NULL), m_pSpotLightDomainShader(NULL), m_pSpotLightPixelShader(NULL),
	m_pSpotLightDomainCB(NULL), m_pSpotLightPixelCB(NULL),
	m_pCapsuleLightVertexShader(NULL), m_pCapsuleLightHullShader(NULL), m_pCapsuleLightDomainShader(NULL), m_pCapsuleLightPixelShader(NULL),
	m_pCapsuleLightDomainCB(NULL), m_pCapsuleLightPixelCB(NULL),
	m_pNoDepthWriteLessStencilMaskState(NULL), m_pNoDepthWriteGreatherStencilMaskState(NULL),
	m_pAdditiveBlendState(NULL), m_pNoDepthClipFrontRS(NULL), m_pWireframeRS(NULL),
	m_pCascadedDepthStencilDSV(NULL), m_pCascadedDepthStencilRT(NULL), m_pCascadedDepthStencilSRV(NULL), m_pCascadedShadowGenRS(NULL), m_fShadowNear(5.f), 
	m_pCascadedShadowGenGeometryShader(NULL), m_pCascadedShadowGenVertexShader(NULL),  m_pCascadedShadowGenGeometryCB(NULL), m_pPCFSamplerState(NULL), m_pShadowGenDepthState(NULL)
{
}

CLightMgr::~CLightMgr()
{
	Release();
}

HRESULT CLightMgr::Initialize()
{
	// Create constant buffers
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_DIRECTIONAL);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pDirLightCB));

	cbDesc.ByteWidth = sizeof(CB_POINT_LIGHT_DOMAIN);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pPointLightDomainCB));

	cbDesc.ByteWidth = sizeof(CB_POINT_LIGHT_PIXEL);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pPointLightPixelCB));

	cbDesc.ByteWidth = sizeof(CB_SPOT_LIGHT_DOMAIN);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSpotLightDomainCB));

	cbDesc.ByteWidth = sizeof(CB_SPOT_LIGHT_PIXEL);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSpotLightPixelCB));

	cbDesc.ByteWidth = sizeof(CB_CAPSULE_LIGHT_DOMAIN);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCapsuleLightDomainCB));

	cbDesc.ByteWidth = sizeof(CB_CAPSULE_LIGHT_PIXEL);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCapsuleLightPixelCB));

	cbDesc.ByteWidth = 3 * sizeof(D3DXMATRIX);
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCascadedShadowGenGeometryCB));

	m_pDirLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"DirLightVS");
	m_pDirLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"DirLightPS");

	m_pPointLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightVS");
	m_pPointLightHullShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightHS");
	m_pPointLightDomainShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightDS");
	m_pPointLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightPS");

	m_pSpotLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"SpotLightVS");
	m_pSpotLightHullShader = CShaderMgr::GetInstance()->Clone_Shader(L"SpotLightHS");
	m_pSpotLightDomainShader = CShaderMgr::GetInstance()->Clone_Shader(L"SpotLightDS");
	m_pSpotLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"SpotLightPS");

	m_pCapsuleLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"CapsuleLightVS");
	m_pCapsuleLightHullShader = CShaderMgr::GetInstance()->Clone_Shader(L"CapsuleLightHS");
	m_pCapsuleLightDomainShader = CShaderMgr::GetInstance()->Clone_Shader(L"CapsuleLightDS");
	m_pCapsuleLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"CapsuleLightPS");

	m_pCascadedShadowGenVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointShadowGenVS");
	m_pCascadedShadowGenGeometryShader = CShaderMgr::GetInstance()->Clone_Shader(L"CascadedShadowMapsGenGS");

	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
	descDepth.FrontFace = noSkyStencilOp;
	descDepth.BackFace = noSkyStencilOp;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_pNoDepthWriteLessStencilMaskState));

	descDepth.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_pNoDepthWriteGreatherStencilMaskState));

	// Create the additive blend state
	D3D11_BLEND_DESC descBlend;
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;
	const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		TRUE,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL,
	};

	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		descBlend.RenderTarget[i] = defaultRenderTargetBlendDesc;
	}
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateBlendState(&descBlend, &m_pAdditiveBlendState));

	D3D11_RASTERIZER_DESC descRast = {
		D3D11_FILL_SOLID,
		D3D11_CULL_FRONT,
		FALSE,
		D3D11_DEFAULT_DEPTH_BIAS,
		D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		FALSE,
		FALSE,
		FALSE,
		FALSE
	};
	descRast.CullMode = D3D11_CULL_FRONT;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRasterizerState(&descRast, &m_pNoDepthClipFrontRS));

	descRast.DepthClipEnable = TRUE;
	descRast.CullMode = D3D11_CULL_BACK;
	//descRast.CullMode = D3D11_CULL_NONE;
	descRast.FillMode = D3D11_FILL_WIREFRAME;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRasterizerState(&descRast, &m_pWireframeRS));

	descRast.FillMode = D3D11_FILL_SOLID;
	descRast.DepthBias = 6;
	descRast.SlopeScaledDepthBias = 1.0f;
	descRast.DepthClipEnable = FALSE;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateRasterizerState(&descRast, &m_pCascadedShadowGenRS));

	// Allocate the depth stencil target
	D3D11_TEXTURE2D_DESC dtd = {
		m_iShadowMapSize, //UINT Width;
		m_iShadowMapSize, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		   //DXGI_FORMAT_R24G8_TYPELESS,
		   DXGI_FORMAT_R32_TYPELESS, //DXGI_FORMAT Format;
		   1, //DXGI_SAMPLE_DESC SampleDesc;
		   0,
		   D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		   D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		   0,//UINT CPUAccessFlags;
		   0//UINT MiscFlags;    
	};

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthView =
	{
		// DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT,
		D3D11_DSV_DIMENSION_TEXTURE2DARRAY,
		0
	};
	D3D11_SHADER_RESOURCE_VIEW_DESC descShaderView =
	{
		//DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		DXGI_FORMAT_R32_FLOAT,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	descShaderView.Texture2D.MipLevels = 1;

	descDepthView.Texture2DArray.FirstArraySlice = 0;
	descDepthView.Texture2DArray.ArraySize = 6;
	descShaderView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	descShaderView.TextureCube.MipLevels = 1;
	descShaderView.TextureCube.MostDetailedMip = 0;

	// Allocate the cascaded shadow maps targets and views
	dtd.ArraySize = CCascadedMatrixSet::m_iTotalCascades;
	dtd.MiscFlags = 0;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateTexture2D(&dtd, NULL, &m_pCascadedDepthStencilRT));

	descDepthView.Texture2DArray.ArraySize = CCascadedMatrixSet::m_iTotalCascades;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilView(m_pCascadedDepthStencilRT, &descDepthView, &m_pCascadedDepthStencilDSV));

	descShaderView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	descShaderView.Texture2DArray.FirstArraySlice = 0;
	descShaderView.Texture2DArray.ArraySize = CCascadedMatrixSet::m_iTotalCascades;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateShaderResourceView(m_pCascadedDepthStencilRT, &descShaderView, &m_pCascadedDepthStencilSRV));

	m_CascadedMatrixSet.Init(m_iShadowMapSize);

	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateSamplerState(&samDesc, &m_pPCFSamplerState));

	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.StencilEnable = FALSE;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	FAILED_CHECK(CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_pShadowGenDepthState));

	return S_OK;
}

void CLightMgr::Release()
{
	::Safe_Delete(m_pDirLightVertexShader);
	::Safe_Delete(m_pDirLightPixelShader);
	::Safe_Delete(m_pPointLightVertexShader);
	::Safe_Delete(m_pPointLightHullShader);
	::Safe_Delete(m_pPointLightDomainShader);
	::Safe_Delete(m_pPointLightPixelShader);

	::Safe_Delete(m_pSpotLightVertexShader);
	::Safe_Delete(m_pSpotLightHullShader);
	::Safe_Delete(m_pSpotLightDomainShader);
	::Safe_Delete(m_pSpotLightPixelShader);
	::Safe_Delete(m_pCapsuleLightVertexShader);
	::Safe_Delete(m_pCapsuleLightHullShader);
	::Safe_Delete(m_pCapsuleLightDomainShader);
	::Safe_Delete(m_pCapsuleLightPixelShader);
	Safe_Release(m_pDirLightCB);

	Safe_Release(m_pPointLightDomainCB);
	Safe_Release(m_pPointLightPixelCB);

	Safe_Release(m_pSpotLightDomainCB);
	Safe_Release(m_pSpotLightPixelCB);

	Safe_Release(m_pCapsuleLightDomainCB);
	Safe_Release(m_pCapsuleLightPixelCB);

	Safe_Release(m_pNoDepthWriteLessStencilMaskState);
	Safe_Release(m_pNoDepthWriteGreatherStencilMaskState);
	Safe_Release(m_pAdditiveBlendState);
	Safe_Release(m_pNoDepthClipFrontRS);
	Safe_Release(m_pWireframeRS);
	
	Safe_Release(m_pCascadedDepthStencilDSV);
	Safe_Release(m_pCascadedDepthStencilRT);
	Safe_Release(m_pCascadedDepthStencilSRV);
	Safe_Release(m_pCascadedShadowGenRS);

	Safe_Release(m_pCascadedShadowGenGeometryCB);
	Safe_Release(m_pPCFSamplerState);
	::Safe_Delete(m_pCascadedShadowGenVertexShader);
	::Safe_Delete(m_pCascadedShadowGenGeometryShader);

	Safe_Release(m_pShadowGenDepthState);
}

void CLightMgr::DoLighting(ID3D11DeviceContext * pd3dImmediateContext, CMultiRenderTarget* pGBuffer, CMultiRenderTarget* pLineMRT)
{
	pd3dImmediateContext->PSSetSamplers(2, 1, &m_pPCFSamplerState);

	// Store the previous depth state
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	pd3dImmediateContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set the depth state for the directional light
	pd3dImmediateContext->OMSetDepthStencilState(m_pNoDepthWriteLessStencilMaskState, 1);

	// Set the GBuffer views
	ID3D11ShaderResourceView* arrViews[5] = { pGBuffer->GetDepthView(), pGBuffer->GetRT(0)->GetSRV(), pGBuffer->GetRT(1)->GetSRV() , pGBuffer->GetRT(2)->GetSRV(), pLineMRT->GetRT(0)->GetSRV() };
	pd3dImmediateContext->PSSetShaderResources(0, 5, arrViews);

	// Do the directional light
	DirectionalLight(pd3dImmediateContext);

	// Once we are done with the directional light, turn on the blending
	ID3D11BlendState* pPrevBlendState;
	FLOAT prevBlendFactor[4];
	UINT prevSampleMask;
	pd3dImmediateContext->OMGetBlendState(&pPrevBlendState, prevBlendFactor, &prevSampleMask);
	pd3dImmediateContext->OMSetBlendState(m_pAdditiveBlendState, prevBlendFactor, prevSampleMask);

	// Set the depth state for the rest of the lights
	pd3dImmediateContext->OMSetDepthStencilState(m_pNoDepthWriteGreatherStencilMaskState, 1);

	ID3D11RasterizerState* pPrevRSState;
	pd3dImmediateContext->RSGetState(&pPrevRSState);
	pd3dImmediateContext->RSSetState(m_pNoDepthClipFrontRS);

	// Do the rest of the lights
	for (std::vector<LIGHT>::iterator itrCurLight = m_arrLights.begin(); itrCurLight != m_arrLights.end(); itrCurLight++)
	{
		if ((*itrCurLight).eLightType == TYPE_POINT)
		{
			PointLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).fRange, (*itrCurLight).vColor);
		}
		else if ((*itrCurLight).eLightType == TYPE_SPOT)
		{
			SpotLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).vDirection, (*itrCurLight).fRange, (*itrCurLight).fInnerAngle, (*itrCurLight).fOuterAngle, (*itrCurLight).vColor);
		}
		else if ((*itrCurLight).eLightType == TYPE_CAPSULE)
		{
			CapsuleLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).vDirection, (*itrCurLight).fRange, (*itrCurLight).fLength, (*itrCurLight).vColor);
		}
	}
	// Cleanup
	pd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->HSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->DSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);

	// Restore the states
	pd3dImmediateContext->OMSetBlendState(pPrevBlendState, prevBlendFactor, prevSampleMask);
	Safe_Release(pPrevBlendState);
	pd3dImmediateContext->RSSetState(pPrevRSState);
	Safe_Release(pPrevRSState);
	pd3dImmediateContext->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	Safe_Release(pPrevDepthState);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pd3dImmediateContext->PSSetShaderResources(0, 4, arrViews);
}


void CLightMgr::DoDebugLightVolume(ID3D11DeviceContext * pd3dImmediateContext)
{
	ID3D11RasterizerState* pPrevRSState;
	pd3dImmediateContext->RSGetState(&pPrevRSState);
	pd3dImmediateContext->RSSetState(m_pWireframeRS);

	for (vector<LIGHT>::iterator itrCurLight = m_arrLights.begin(); itrCurLight != m_arrLights.end(); itrCurLight++)
	{
		if ((*itrCurLight).eLightType == TYPE_POINT)
		{
			PointLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).fRange, (*itrCurLight).vColor/*, true*/);
		}
	}

	// Cleanup
	pd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->HSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->DSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);

	// Restore the states
	pd3dImmediateContext->RSSetState(pPrevRSState);
	Safe_Release(pPrevRSState);
}


void CLightMgr::SetAmbient(const D3DXVECTOR3& vAmbientLowerColor, const D3DXVECTOR3& vAmbientUpperColor)
{
	m_vAmbientLowerColor = vAmbientLowerColor;
	m_vAmbientUpperColor = vAmbientUpperColor;
}

void CLightMgr::SetDirectional(const D3DXVECTOR3& vDirectionalDir, const D3DXVECTOR3& vDirectionalColor)
{
	D3DXVec3Normalize(&m_vDirectionalDir, &vDirectionalDir);
	m_vDirectionalColor = vDirectionalColor;
}

void CLightMgr::AddPointLight(const D3DXVECTOR3& vPointPosition, float fPointRange, const D3DXVECTOR3& vPointColor)
{
	LIGHT pointLight;

	pointLight.eLightType = TYPE_POINT;
	pointLight.vPosition = vPointPosition;
	pointLight.fRange = fPointRange;
	pointLight.vColor = vPointColor;

	m_arrLights.push_back(pointLight);
}

void CLightMgr::AddSpotLight(const D3DXVECTOR3& vSpotPosition, const D3DXVECTOR3& vSpotDirection, float fSpotRange, float fSpotOuterAngle, float fSpotInnerAngle, const D3DXVECTOR3& vSpotColor)
{
	LIGHT spotLight;

	spotLight.eLightType = TYPE_SPOT;
	spotLight.vPosition = vSpotPosition;
	spotLight.vDirection = vSpotDirection;
	spotLight.fRange = fSpotRange;
	spotLight.fOuterAngle = D3DX_PI * fSpotOuterAngle / 180.0f;
	spotLight.fInnerAngle = D3DX_PI * fSpotInnerAngle / 180.0f;
	spotLight.vColor = vSpotColor;

	m_arrLights.push_back(spotLight);
}

void CLightMgr::AddCapsuleLight(const D3DXVECTOR3 & vCapsulePosition, const D3DXVECTOR3 & vCapsuleDirection, float fCapsuleRange, float fCapsuleLength, const D3DXVECTOR3 & vCapsuleColor)
{
	LIGHT capsuleLight;

	capsuleLight.eLightType = TYPE_CAPSULE;
	capsuleLight.vPosition = vCapsulePosition;
	capsuleLight.vDirection = vCapsuleDirection;
	capsuleLight.fRange = fCapsuleRange;
	capsuleLight.fLength = fCapsuleLength;
	capsuleLight.vColor = vCapsuleColor;

	m_arrLights.push_back(capsuleLight);
}

void CLightMgr::PrepareNextShadowLight(ID3D11DeviceContext * pd3dImmediateContext)
{
	pd3dImmediateContext->OMSetDepthStencilState(m_pShadowGenDepthState, 0);

	pd3dImmediateContext->RSSetState(m_pCascadedShadowGenRS);

	HRESULT hr;

	D3D11_VIEWPORT vp[3] = { { 0, 0, m_iShadowMapSize, m_iShadowMapSize, 0.0f, 1.0f },{ 0, 0, m_iShadowMapSize, m_iShadowMapSize, 0.0f, 1.0f },{ 0, 0, m_iShadowMapSize, m_iShadowMapSize, 0.0f, 1.0f } };
	pd3dImmediateContext->RSSetViewports(3, vp);
	
	// Set the depth target
	ID3D11RenderTargetView* nullRT = NULL;
	pd3dImmediateContext->OMSetRenderTargets(1, &nullRT, m_pCascadedDepthStencilDSV);

	// Clear the depth stencil
	pd3dImmediateContext->ClearDepthStencilView(m_pCascadedDepthStencilDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	// Get the cascade matrices for the current camera configuration
	m_CascadedMatrixSet.Update(m_vDirectionalDir);

	// Fill the shadow generation matrices constant buffer23
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dImmediateContext->Map(m_pCascadedShadowGenGeometryCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	D3DXMATRIX* pCascadeShadowGenMat = (D3DXMATRIX*)MappedResource.pData;

	for (int i = 0; i < CCascadedMatrixSet::m_iTotalCascades; i++)
	{
		D3DXMatrixTranspose(&pCascadeShadowGenMat[i], m_CascadedMatrixSet.GetWorldToCascadeProj(i));
	}

	pd3dImmediateContext->Unmap(m_pCascadedShadowGenGeometryCB, 0);
	pd3dImmediateContext->GSSetConstantBuffers(0, 1, &m_pCascadedShadowGenGeometryCB);

	// Set the vertex layout
	pd3dImmediateContext->IASetInputLayout(m_pCascadedShadowGenVertexShader->m_pVertexLayout);

	// Set the shadow generation shaders
	pd3dImmediateContext->VSSetShader(m_pCascadedShadowGenVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(m_pCascadedShadowGenGeometryShader->m_pGeometryShader, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);
}

void CLightMgr::DirectionalLight(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Fill the directional and ambient values constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pDirLightCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_DIRECTIONAL* pDirectionalValuesCB = (CB_DIRECTIONAL*)MappedResource.pData;
	pDirectionalValuesCB->vAmbientLower = GammaToLinear(m_vAmbientLowerColor);
	pDirectionalValuesCB->vAmbientRange = GammaToLinear(m_vAmbientUpperColor) - GammaToLinear(m_vAmbientLowerColor);
	pDirectionalValuesCB->vDirToLight = -m_vDirectionalDir;
	pDirectionalValuesCB->vDirectionalColor = GammaToLinear(m_vDirectionalColor);

	D3DXMatrixTranspose(&pDirectionalValuesCB->ToShadowSpace, m_CascadedMatrixSet.GetWorldToShadowSpace());

	pDirectionalValuesCB->ToCascadeSpace[0] = m_CascadedMatrixSet.GetToCascadeOffsetX();
	pDirectionalValuesCB->ToCascadeSpace[1] = m_CascadedMatrixSet.GetToCascadeOffsetY();
	pDirectionalValuesCB->ToCascadeSpace[2] = m_CascadedMatrixSet.GetToCascadeScale();

	pd3dImmediateContext->Unmap(m_pDirLightCB, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &m_pDirLightCB);

	pd3dImmediateContext->PSSetShaderResources(5, 1, &m_pCascadedDepthStencilSRV);

	// Primitive settings
	pd3dImmediateContext->IASetInputLayout(NULL);
	pd3dImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set the shaders
	pd3dImmediateContext->VSSetShader(m_pDirLightVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pDirLightPixelShader->m_pPixelShader, NULL, 0);
	// gpu에게 현재 버텍스버퍼, 버텍스레이아웃, 프리미티브 토폴로지를 사용해서 렌더링하라는 함수
	pd3dImmediateContext->Draw(
		4, // gpu로 보낸 버텍스의 갯수
		0); // 처음시작할 버텍스의 인덱스

			// Cleanup
	ID3D11ShaderResourceView *arrRV[1] = { NULL };
	pd3dImmediateContext->PSSetShaderResources(4, 1, arrRV);
	pd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CLightMgr::PointLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, float fRange, const D3DXVECTOR3& vColor)
{
	D3DXMATRIX mLightWorldScale;
	D3DXMatrixScaling(&mLightWorldScale, fRange, fRange, fRange);
	D3DXMATRIX mLightWorldTrans;
	D3DXMatrixTranslation(&mLightWorldTrans, vPos.x, vPos.y, vPos.z);
	D3DXMATRIX mView = *(CCamera::GetInstance()->GetViewMatrix());
	D3DXMATRIX mProj = *(CCamera::GetInstance()->GetProjMatrix());
	D3DXMATRIX mWorldViewProjection = mLightWorldScale * mLightWorldTrans * mView * mProj;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pPointLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_POINT_LIGHT_DOMAIN* pPointLightDomainCB = (CB_POINT_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pPointLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pd3dImmediateContext->Unmap(m_pPointLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pPointLightDomainCB);

	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pPointLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_POINT_LIGHT_PIXEL* pPointLightPixelCB = (CB_POINT_LIGHT_PIXEL*)MappedResource.pData;
	pPointLightPixelCB->PointLightPos = vPos;
	pPointLightPixelCB->PointLightRangeRcp = 1.0f / fRange;
	pPointLightPixelCB->PointColor = GammaToLinear(vColor);
	pd3dImmediateContext->Unmap(m_pPointLightPixelCB, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &m_pPointLightPixelCB);

	pd3dImmediateContext->IASetInputLayout(NULL);
	pd3dImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	// Set the shaders
	pd3dImmediateContext->VSSetShader(m_pPointLightVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->HSSetShader(m_pPointLightHullShader->m_pHullShader, NULL, 0);
	pd3dImmediateContext->DSSetShader(m_pPointLightDomainShader->m_pDomainShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pPointLightPixelShader->m_pPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(2, 0);
}

void CLightMgr::SpotLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, float fRange, float fInnerAngle, float fOuterAngle, const D3DXVECTOR3& vColor)
{
	// Convert angle in radians to sin/cos values
	float fCosInnerAngle = cosf(fInnerAngle);
	float fSinOuterAngle = sinf(fOuterAngle);
	float fCosOuterAngle = cosf(fOuterAngle);

	// Scale matrix from the cone local space to the world angles and range
	D3DXMATRIX mLightWorldScale;
	D3DXMatrixScaling(&mLightWorldScale, fRange, fRange, fRange);

	// Rotate and translate matrix from cone local space to lights world space
	D3DXVECTOR3 vUp = (vDir.y > 0.9 || vDir.y < -0.9) ? D3DXVECTOR3(0.0f, 0.0f, vDir.y) : D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vDir);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Cross(&vUp, &vDir, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVECTOR3 vAt = vPos + vDir * fRange;
	D3DXMATRIX m_LightWorldTransRotate;
	D3DXMatrixIdentity(&m_LightWorldTransRotate);
	for (int i = 0; i < 3; i++)
	{
		m_LightWorldTransRotate.m[0][i] = (&vRight.x)[i];
		m_LightWorldTransRotate.m[1][i] = (&vUp.x)[i];
		m_LightWorldTransRotate.m[2][i] = (&vDir.x)[i];
		m_LightWorldTransRotate.m[3][i] = (&vPos.x)[i];
	}

	// Prepare the combined local to projected space matrix
	D3DXMATRIX mView = *(CCamera::GetInstance()->GetViewMatrix());
	D3DXMATRIX mProj = *(CCamera::GetInstance()->GetProjMatrix());
	D3DXMATRIX mWorldViewProjection = mLightWorldScale * m_LightWorldTransRotate * mView * mProj;

	// Write the matrix to the domain shader constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pSpotLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_SPOT_LIGHT_DOMAIN* pPointLightDomainCB = (CB_SPOT_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pPointLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pPointLightDomainCB->fSinAngle = fSinOuterAngle;
	pPointLightDomainCB->fCosAngle = fCosOuterAngle;
	pd3dImmediateContext->Unmap(m_pSpotLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pSpotLightDomainCB);

	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pSpotLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_SPOT_LIGHT_PIXEL* pPointLightPixelCB = (CB_SPOT_LIGHT_PIXEL*)MappedResource.pData;
	pPointLightPixelCB->SpotLightPos = vPos;
	pPointLightPixelCB->SpotLightRangeRcp = 1.0f / fRange;
	pPointLightPixelCB->vDirToLight = -vDir;
	pPointLightPixelCB->SpotCosOuterCone = fCosOuterAngle;
	pPointLightPixelCB->SpotColor = GammaToLinear(vColor);
	pPointLightPixelCB->SpotCosConeAttRange = fCosInnerAngle - fCosOuterAngle;
	pd3dImmediateContext->Unmap(m_pSpotLightPixelCB, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &m_pSpotLightPixelCB);

	pd3dImmediateContext->IASetInputLayout(NULL);
	pd3dImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	// Set the shaders
	pd3dImmediateContext->VSSetShader(m_pSpotLightVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->HSSetShader(m_pSpotLightHullShader->m_pHullShader, NULL, 0);
	pd3dImmediateContext->DSSetShader(m_pSpotLightDomainShader->m_pDomainShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pSpotLightPixelShader->m_pPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(1, 0);
}

void CLightMgr::CapsuleLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, float fRange, float fLen, const D3DXVECTOR3& vColor)
{
	// Rotate and translate matrix from capsule local space to lights world space
	D3DXVECTOR3 vUp = (vDir.y > 0.9 || vDir.y < -0.9) ? D3DXVECTOR3(0.0f, 0.0f, vDir.y) : D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vDir);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Cross(&vUp, &vDir, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVECTOR3 vCenterPos = vPos + 0.5f * vDir * fLen;
	D3DXVECTOR3 vAt = vCenterPos + vDir * fLen;
	D3DXMATRIX m_LightWorldTransRotate;
	D3DXMatrixIdentity(&m_LightWorldTransRotate);
	for (int i = 0; i < 3; i++)
	{
		m_LightWorldTransRotate.m[0][i] = (&vRight.x)[i];
		m_LightWorldTransRotate.m[1][i] = (&vUp.x)[i];
		m_LightWorldTransRotate.m[2][i] = (&vDir.x)[i];
		m_LightWorldTransRotate.m[3][i] = (&vCenterPos.x)[i];
	}

	D3DXMATRIX mView = *(CCamera::GetInstance()->GetViewMatrix());
	D3DXMATRIX mProj = *(CCamera::GetInstance()->GetProjMatrix());
	D3DXMATRIX mWorldViewProjection = m_LightWorldTransRotate * mView * mProj;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pCapsuleLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_CAPSULE_LIGHT_DOMAIN* pCapsuleLightDomainCB = (CB_CAPSULE_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pCapsuleLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pCapsuleLightDomainCB->HalfCapsuleLen = 0.5f * fLen;
	pCapsuleLightDomainCB->CapsuleRange = fRange;
	pd3dImmediateContext->Unmap(m_pCapsuleLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pCapsuleLightDomainCB);

	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pCapsuleLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_CAPSULE_LIGHT_PIXEL* pCapsuleLightPixelCB = (CB_CAPSULE_LIGHT_PIXEL*)MappedResource.pData;
	pCapsuleLightPixelCB->CapsuleLightPos = vPos;
	pCapsuleLightPixelCB->CapsuleLightRangeRcp = 1.0f / fRange;
	pCapsuleLightPixelCB->CapsuleDir = vDir;
	pCapsuleLightPixelCB->CapsuleLen = fLen;
	pCapsuleLightPixelCB->CapsuleColor = GammaToLinear(vColor);
	pd3dImmediateContext->Unmap(m_pCapsuleLightPixelCB, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &m_pCapsuleLightPixelCB);

	pd3dImmediateContext->IASetInputLayout(NULL);
	pd3dImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	// Set the shaders
	pd3dImmediateContext->VSSetShader(m_pCapsuleLightVertexShader->m_pVertexShader, NULL, 0);
	pd3dImmediateContext->HSSetShader(m_pCapsuleLightHullShader->m_pHullShader, NULL, 0);
	pd3dImmediateContext->DSSetShader(m_pCapsuleLightDomainShader->m_pDomainShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pCapsuleLightPixelShader->m_pPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(2, 0);
}