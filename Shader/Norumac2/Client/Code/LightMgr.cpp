#include "stdafx.h"
#include "LightMgr.h"

#include "Device.h"
#include "Camera.h"
#include "ShaderMgr.h"

extern CGBuffer g_GBuffer;

// Helpers
HRESULT CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);

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
#pragma pack(pop)

CLightMgr::CLightMgr() :m_bShowLightVolume(false),
m_pDirLightVertexShader(NULL), m_pDirLightPixelShader(NULL), m_pDirLightCB(NULL),
m_pPointLightVertexShader(NULL), m_pPointLightHullShader(NULL), m_pPointLightDomainShader(NULL), m_pPointLightPixelShader(NULL),
m_pPointLightDomainCB(NULL), m_pPointLightPixelCB(NULL),
m_pSpotLightVertexShader(NULL), m_pSpotLightHullShader(NULL), m_pSpotLightDomainShader(NULL), m_pSpotLightPixelShader(NULL),
m_pSpotLightDomainCB(NULL), m_pSpotLightPixelCB(NULL),
m_pDebugLightPixelShader(NULL),
m_pNoDepthWriteLessStencilMaskState(NULL), m_pNoDepthWriteGreatherStencilMaskState(NULL),
m_pAdditiveBlendState(NULL), m_pNoDepthClipFrontRS(NULL), m_pWireframeRS(NULL)
{
}

CLightMgr::~CLightMgr()
{

}

HRESULT CLightMgr::Init()
{
	HRESULT hr;

	// Create constant buffers
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_DIRECTIONAL);

	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pDirLightCB);
	if (FAILED(hr))
		return hr;
	//DXUT_SetDebugName(m_pDirLightCB, "Directional Light CB");
	m_pDirLightCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Directional Light CB"), "Directional Light CB");

	cbDesc.ByteWidth = sizeof(CB_POINT_LIGHT_DOMAIN);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pPointLightDomainCB);
	if (FAILED(hr))
		return hr;
	m_pPointLightDomainCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light Domain CB"), "Point Light Domain CB");

	cbDesc.ByteWidth = sizeof(CB_POINT_LIGHT_PIXEL);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pPointLightPixelCB);
	if (FAILED(hr))
		return hr;
	m_pPointLightPixelCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light Pixel CB"), "Point Light Pixel CB");

	cbDesc.ByteWidth = sizeof(CB_SPOT_LIGHT_DOMAIN);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSpotLightDomainCB);
	if (FAILED(hr))
		return hr;
	m_pSpotLightDomainCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light Domain CB"), "Spot Light Domain CB");

	cbDesc.ByteWidth = sizeof(CB_SPOT_LIGHT_PIXEL);
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSpotLightPixelCB);
	if (FAILED(hr))
		return hr;
	m_pSpotLightPixelCB->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light Pixel CB"), "Spot Light Pixel CB");

	// Read the HLSL file
	WCHAR str[MAX_PATH];
	hr = (DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"DirLight.fx"));
	//hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"../ShaderCode/Shader.fx", "VS", "vs_5_0", SHADER_VS);

	// Compile the shaders
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// Load the directional light shaders
	ID3DBlob* pShaderBlob = NULL;
	hr = CompileShader(str, NULL, "DirLightVS", "vs_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;

	hr = CDevice::GetInstance()->m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDirLightVertexShader);
	if (FAILED(hr))
		return hr;
	m_pDirLightVertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Directional Light VS"), "Directional Light VS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "DirLightPS", "ps_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDirLightPixelShader);
	if (FAILED(hr))
		return hr;
	m_pDirLightPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Directional Light PS"), "Directional Light PS");
	Safe_Release(pShaderBlob);

	// Load the point light shaders
	hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"PointLight.fx");
	if (FAILED(hr))
		return hr;
	hr = CompileShader(str, NULL, "PointLightVS", "vs_5_0", dwShaderFlags, &pShaderBlob);
	hr = CDevice::GetInstance()->m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pPointLightVertexShader);
	if (FAILED(hr))
		return hr;
	m_pPointLightVertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light VS"), "Point Light VS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "PointLightHS", "hs_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreateHullShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pPointLightHullShader);
	if (FAILED(hr))
		return hr;
	m_pPointLightHullShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light HS"), "Point Light HS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "PointLightDS", "ds_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreateDomainShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pPointLightDomainShader);
	if (FAILED(hr))
		return hr;
	m_pPointLightDomainShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light DS"), "Point Light DS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "PointLightPS", "ps_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pPointLightPixelShader);
	if (FAILED(hr))
		return hr;
	m_pPointLightPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Point Light PS"), "Point Light PS");

	// Load the spot light shaders
	hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"SpotLight.fx");
	if (FAILED(hr))
		return hr;
	hr = CompileShader(str, NULL, "SpotLightVS", "vs_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pSpotLightVertexShader);
	if (FAILED(hr))
		return hr;
	m_pSpotLightVertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light VS"), "Spot Light VS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "SpotLightHS", "hs_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreateHullShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pSpotLightHullShader);
	if (FAILED(hr))
		return hr;
	m_pSpotLightHullShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light HS"), "Spot Light HS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "SpotLightDS", "ds_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreateDomainShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pSpotLightDomainShader);
	if (FAILED(hr))
		return hr;
	m_pSpotLightDomainShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light DS"), "Spot Light DS");
	Safe_Release(pShaderBlob);

	hr = CompileShader(str, NULL, "SpotLightPS", "ps_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pSpotLightPixelShader);
	if (FAILED(hr))
		return hr;
	m_pSpotLightPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Spot Light PS"), "Spot Light PS");
	Safe_Release(pShaderBlob);

	// Load the light volume debug shader
	hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"Common.fx");
	if (FAILED(hr))
		return hr;
	hr = CompileShader(str, NULL, "DebugLightPS", "ps_5_0", dwShaderFlags, &pShaderBlob);
	if (FAILED(hr))
		return hr;
	hr = CDevice::GetInstance()->m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDebugLightPixelShader);
	if (FAILED(hr))
		return hr;
	m_pDebugLightPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Debug Light Volume PS"), "Debug Light Volume PS");
	Safe_Release(pShaderBlob);

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
	hr = CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_pNoDepthWriteLessStencilMaskState);
	if (FAILED(hr))
		return hr;
	m_pNoDepthWriteLessStencilMaskState->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Depth Test Less / No Write, Stencil Mask DS"), "Depth Test Less / No Write, Stencil Mask DS");

	descDepth.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	hr = CDevice::GetInstance()->m_pDevice->CreateDepthStencilState(&descDepth, &m_pNoDepthWriteGreatherStencilMaskState);
	if (FAILED(hr))
		return hr;
	m_pNoDepthWriteGreatherStencilMaskState->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Depth Test Greather / No Write, Stencil Mask DS"), "Depth Test Greather / No Write, Stencil Mask DS");

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
		descBlend.RenderTarget[i] = defaultRenderTargetBlendDesc;
	hr = CDevice::GetInstance()->m_pDevice->CreateBlendState(&descBlend, &m_pAdditiveBlendState);
	if (FAILED(hr))
		return hr;
	m_pAdditiveBlendState->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Additive Alpha BS"), "Additive Alpha BS");

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
	hr = CDevice::GetInstance()->m_pDevice->CreateRasterizerState(&descRast, &m_pNoDepthClipFrontRS);
	if (FAILED(hr))
		return hr;
	m_pNoDepthClipFrontRS->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("No Depth Clip Front RS"), "No Depth Clip Front RS");

	descRast.DepthClipEnable = TRUE;
	descRast.CullMode = D3D11_CULL_BACK;
	//descRast.CullMode = D3D11_CULL_NONE;
	descRast.FillMode = D3D11_FILL_WIREFRAME;
	hr = CDevice::GetInstance()->m_pDevice->CreateRasterizerState(&descRast, &m_pWireframeRS);
	if (FAILED(hr))
		return hr;
	m_pWireframeRS->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA("Wireframe RS"), "Wireframe RS");

	return hr;
}

void CLightMgr::Deinit()
{
	Safe_Release(m_pDirLightVertexShader);
	Safe_Release(m_pDirLightPixelShader);
	Safe_Release(m_pDirLightCB);
	Safe_Release(m_pPointLightVertexShader);
	Safe_Release(m_pPointLightHullShader);
	Safe_Release(m_pPointLightDomainShader);
	Safe_Release(m_pPointLightPixelShader);
	Safe_Release(m_pPointLightDomainCB);
	Safe_Release(m_pPointLightPixelCB);
	Safe_Release(m_pSpotLightVertexShader);
	Safe_Release(m_pSpotLightHullShader);
	Safe_Release(m_pSpotLightDomainShader);
	Safe_Release(m_pSpotLightPixelShader);
	Safe_Release(m_pSpotLightDomainCB);
	Safe_Release(m_pSpotLightPixelCB);
	Safe_Release(m_pDebugLightPixelShader);
	Safe_Release(m_pNoDepthWriteLessStencilMaskState);
	Safe_Release(m_pNoDepthWriteGreatherStencilMaskState);
	Safe_Release(m_pAdditiveBlendState);
	Safe_Release(m_pNoDepthClipFrontRS);
	Safe_Release(m_pWireframeRS);
}

void CLightMgr::DoLighting(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Store the previous depth state
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	pd3dImmediateContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set the depth state for the directional light
	pd3dImmediateContext->OMSetDepthStencilState(m_pNoDepthWriteLessStencilMaskState, 1);

	// Set the GBuffer views
	ID3D11ShaderResourceView* arrViews[4] = { g_GBuffer.GetDepthView(), g_GBuffer.GetColorView(), g_GBuffer.GetNormalView() ,g_GBuffer.GetSpecPowerView() };
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
			PointLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).fRange, (*itrCurLight).vColor, false);
		}
		else if ((*itrCurLight).eLightType == TYPE_SPOT)
		{
			SpotLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).vDirection, (*itrCurLight).fRange, (*itrCurLight).fInnerAngle,
				(*itrCurLight).fOuterAngle, (*itrCurLight).vColor, false);
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

void CLightMgr::DoDebugLightVolume(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Set the wireframe mode
	ID3D11RasterizerState* pPrevRSState;
	pd3dImmediateContext->RSGetState(&pPrevRSState);
	pd3dImmediateContext->RSSetState(m_pWireframeRS);

	for (std::vector<LIGHT>::iterator itrCurLight = m_arrLights.begin(); itrCurLight != m_arrLights.end(); itrCurLight++)
	{
		if ((*itrCurLight).eLightType == TYPE_POINT)
		{
			PointLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).fRange, (*itrCurLight).vColor, true);
		}
		else if ((*itrCurLight).eLightType == TYPE_SPOT)
		{
			SpotLight(pd3dImmediateContext, (*itrCurLight).vPosition, (*itrCurLight).vDirection, (*itrCurLight).fRange, (*itrCurLight).fInnerAngle,
				(*itrCurLight).fOuterAngle, (*itrCurLight).vColor, true);
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

void CLightMgr::DirectionalLight(ID3D11DeviceContext* pd3dImmediateContext)
{
	HRESULT hr;

	// Fill the directional and ambient values constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// v???
	pd3dImmediateContext->Map(m_pDirLightCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
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
	pd3dImmediateContext->VSSetShader(m_pDirLightVertexShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pDirLightPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(4, 0);

	// Cleanup
	ID3D11ShaderResourceView *arrRV[1] = { NULL };
	pd3dImmediateContext->PSSetShaderResources(4, 1, arrRV);
	pd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CLightMgr::PointLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, float fRange, const D3DXVECTOR3& vColor, bool bWireframe)
{
	HRESULT hr;

	D3DXMATRIX mLightWorldScale;
	D3DXMatrixScaling(&mLightWorldScale, fRange, fRange, fRange);
	D3DXMATRIX mLightWorldTrans;
	D3DXMatrixTranslation(&mLightWorldTrans, vPos.x, vPos.y, vPos.z);
	D3DXMATRIX mView = CCamera::GetInstance()->m_matView;
	D3DXMATRIX mProj = CCamera::GetInstance()->m_matProj;
	D3DXMATRIX mWorldViewProjection = mLightWorldScale * mLightWorldTrans * mView * mProj;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// v???
	pd3dImmediateContext->Map(m_pPointLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_POINT_LIGHT_DOMAIN* pPointLightDomainCB = (CB_POINT_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pPointLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pd3dImmediateContext->Unmap(m_pPointLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pPointLightDomainCB);

	// v???
	pd3dImmediateContext->Map(m_pPointLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
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
	pd3dImmediateContext->VSSetShader(m_pPointLightVertexShader, NULL, 0);
	pd3dImmediateContext->HSSetShader(m_pPointLightHullShader, NULL, 0);
	pd3dImmediateContext->DSSetShader(m_pPointLightDomainShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(bWireframe ? m_pDebugLightPixelShader : m_pPointLightPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(2, 0);
}

void CLightMgr::SpotLight(ID3D11DeviceContext* pd3dImmediateContext, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, float fRange, float fInnerAngle, float fOuterAngle, const D3DXVECTOR3& vColor, bool bWireframe)
{
	HRESULT hr;

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
	D3DXMATRIX mView = CCamera::GetInstance()->m_matView;
	D3DXMATRIX mProj = CCamera::GetInstance()->m_matProj;
	D3DXMATRIX mWorldViewProjection = mLightWorldScale * m_LightWorldTransRotate * mView * mProj;

	// Write the matrix to the domain shader constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// v???
	pd3dImmediateContext->Map(m_pSpotLightDomainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_SPOT_LIGHT_DOMAIN* pPointLightDomainCB = (CB_SPOT_LIGHT_DOMAIN*)MappedResource.pData;
	D3DXMatrixTranspose(&pPointLightDomainCB->WolrdViewProj, &mWorldViewProjection);
	pPointLightDomainCB->fSinAngle = fSinOuterAngle;
	pPointLightDomainCB->fCosAngle = fCosOuterAngle;
	pd3dImmediateContext->Unmap(m_pSpotLightDomainCB, 0);
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, &m_pSpotLightDomainCB);

	// v???
	pd3dImmediateContext->Map(m_pSpotLightPixelCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
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
	pd3dImmediateContext->VSSetShader(m_pSpotLightVertexShader, NULL, 0);
	pd3dImmediateContext->HSSetShader(m_pSpotLightHullShader, NULL, 0);
	pd3dImmediateContext->DSSetShader(m_pSpotLightDomainShader, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(bWireframe ? m_pDebugLightPixelShader : m_pSpotLightPixelShader, NULL, 0);

	pd3dImmediateContext->Draw(1, 0);
}
