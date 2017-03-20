#include "stdafx.h"
#include "LightMgr.h"
#include "Device.h"
#include "Camera.h"
//#include "TargetMgr.h"
#include "MultiRenderTarget.h"
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
m_pNoDepthWriteLessStencilMaskState(NULL), m_pNoDepthWriteGreatherStencilMaskState(NULL),
m_pAdditiveBlendState(NULL), m_pNoDepthClipFrontRS(NULL), m_pWireframeRS(NULL)
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

	m_pDirLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"DirLightVS");
	m_pDirLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"DirLightPS");

	m_pPointLightVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightVS");
	m_pPointLightHullShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightHS");
	m_pPointLightDomainShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightDS");
	m_pPointLightPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointLightPS");

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

	return S_OK;
}

void CLightMgr::Release()
{
	Safe_Release(m_pDirLightCB);

	Safe_Release(m_pPointLightDomainCB);
	Safe_Release(m_pPointLightPixelCB);

	Safe_Release(m_pNoDepthWriteLessStencilMaskState);
	Safe_Release(m_pNoDepthWriteGreatherStencilMaskState);
	Safe_Release(m_pAdditiveBlendState);
	Safe_Release(m_pNoDepthClipFrontRS);
	Safe_Release(m_pWireframeRS);
}

void CLightMgr::DoLighting(ID3D11DeviceContext * pd3dImmediateContext, CMultiRenderTarget* pGBuffer)
{
	// Store the previous depth state
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	pd3dImmediateContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set the depth state for the directional light
	pd3dImmediateContext->OMSetDepthStencilState(m_pNoDepthWriteLessStencilMaskState, 1);

	// Set the GBuffer views
	ID3D11ShaderResourceView* arrViews[4] = { pGBuffer->GetDepthView(), pGBuffer->GetColorView(), pGBuffer->GetNormalView() , pGBuffer->GetSpecPowerView() };
	pd3dImmediateContext->PSSetShaderResources(0, 4, arrViews);

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

void CLightMgr::DirectionalLight(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Fill the directional and ambient values constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pDirLightCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource),);
	CB_DIRECTIONAL* pDirectionalValuesCB = (CB_DIRECTIONAL*)MappedResource.pData;
	pDirectionalValuesCB->vAmbientLower = GammaToLinear(m_vAmbientLowerColor);
	pDirectionalValuesCB->vAmbientRange = GammaToLinear(m_vAmbientUpperColor) - GammaToLinear(m_vAmbientLowerColor);
	pDirectionalValuesCB->vDirToLight = -m_vDirectionalDir;
	pDirectionalValuesCB->vDirectionalColor = GammaToLinear(m_vDirectionalColor);
	pd3dImmediateContext->Unmap(m_pDirLightCB, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &m_pDirLightCB);

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
	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pPointLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource),);
	CB_POINT_LIGHT_DOMAIN* pPointLightDomainCB = (CB_POINT_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pPointLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pd3dImmediateContext->Unmap(m_pPointLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pPointLightDomainCB);

	FAILED_CHECK_RETURN(pd3dImmediateContext->Map(m_pPointLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource),);
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