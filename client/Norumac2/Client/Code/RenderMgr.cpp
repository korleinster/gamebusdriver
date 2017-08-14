#include "stdafx.h"
#include "RenderMgr.h"
#include "Obj.h"
#include "Scene.h"
#include "Device.h"
#include "TargetMgr.h"
#include "LightMgr.h"
#include "MultiRenderTarget.h"
#include "TimeMgr.h"
#include "Input.h"
#include "camera.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "RcTex.h"
#include "ResourcesMgr.h"
#include "Font.h"

#pragma pack(push,1)
struct CB_LINEAR_DEPTH
{
	D3DXVECTOR4 PerspectiveValues;
};
#pragma pack(pop)

IMPLEMENT_SINGLETON(CRenderMgr)

CRenderMgr::CRenderMgr()
	:m_fTime(0.f)
	, m_dwCount(0)
	, m_pDevice(CDevice::GetInstance())
	, m_pTargetMgr(CTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
	, m_bRenderGBuffer(false)
	, m_bDefferdOn(true)
	, m_fTimeCheck(0.f)
	, m_pBorderLineVS(nullptr)
	, m_pBorderLinePS(nullptr)
	, m_pRcTex(nullptr)
	, m_pSamplerState(nullptr)
	, m_pLinearDepthCB(nullptr)
	, m_fSobelValue(0.0667f)
	, m_pFPSFont(nullptr)
{
	ZeroMemory(m_szFps, sizeof(TCHAR) * 128);

	m_pTargetMgr->Initialize();
	m_pLightMgr->Initialize();

	// Dir
	m_fDirColor[RGB_RED] = 1.2f;
	m_fDirColor[RGB_GREEN] = 1.2f;
	m_fDirColor[RGB_BLUE] = 1.2f;

	// Point
	m_fPointColor[RGB_RED] = 1.5f;
	m_fPointColor[RGB_GREEN] = 1.0f;
	m_fPointColor[RGB_BLUE] = 0.0f;
	m_fPointY = 2.f;
	m_fPointRadius = 11.f;

	m_vDirLight = D3DXVECTOR3(-0.62f, -0.44f, -0.64f);

	m_pLightMgr->SetAmbient(D3DXVECTOR3(0.4f, 0.4f, 0.4f), D3DXVECTOR3(0.4f, 0.4f, 0.4f));
	m_pLightMgr->SetDirectional(
		D3DXVECTOR3(m_vDirLight.x, m_vDirLight.y, m_vDirLight.z),
		D3DXVECTOR3(m_fDirColor[RGB_RED], m_fDirColor[RGB_GREEN], m_fDirColor[RGB_BLUE]));

	m_pBorderLineVS = CShaderMgr::GetInstance()->Clone_Shader(L"BorderLineVS");
	m_pBorderLinePS = CShaderMgr::GetInstance()->Clone_Shader(L"BorderLinePS");

	m_pRcTex = dynamic_cast<CVIBuffer*>(CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex"));

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

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_LINEAR_DEPTH);
	CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pLinearDepthCB);

	m_pFPSFont = CFont::Create(L"Font_Star");


	m_pFPSFont->m_eType = FONT_TYPE_OUTLINE;
	m_pFPSFont->m_wstrText = L"";
	m_pFPSFont->m_fSize = 30.f;
	m_pFPSFont->m_nColor = 0xFF0000FF;
	m_pFPSFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFPSFont->m_vPos = D3DXVECTOR2(750.f, 50.f);
	m_pFPSFont->m_fOutlineSize = 1.f;
	m_pFPSFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

	//InitLight();
}


CRenderMgr::~CRenderMgr()
{
	m_pTargetMgr->DestroyInstance();
	m_pLightMgr->DestroyInstance();

	Safe_Delete(m_pBorderLineVS);
	Safe_Delete(m_pBorderLinePS);
	Safe_Delete(m_pRcTex);
	Safe_Delete(m_pFPSFont);

	Safe_Release(m_pSamplerState);
	Safe_Release(m_pLinearDepthCB);
}

void CRenderMgr::SetCurrentScene(CScene * pScene)
{
}

void CRenderMgr::AddRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	NULL_CHECK_RETURN(pObj, );

	m_RenderGroup[eRednerID].push_back(pObj);
}

void CRenderMgr::DelRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	list<CObj*>::iterator	iter = m_RenderGroup[eRednerID].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[eRednerID].end();

	for (; iter != iter_end; )
	{
		if ((*iter) == pObj)
			iter = m_RenderGroup[eRednerID].erase(iter);
		else
			++iter;

	}
}

HRESULT CRenderMgr::InitScene(void)
{
	return S_OK;
}

void CRenderMgr::InitPointLight()
{
	// 조명 클리어
	m_pLightMgr->ClearLights();

	// 파랑
	m_pLightMgr->AddPointLight(D3DXVECTOR3(256.161f, 15.f + m_fPointY, 319.372f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(264.109f, 15.f + m_fPointY, 318.953f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(272.109f, 12.f + m_fPointY, 318.553f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(248.109f, 12.f + m_fPointY, 319.773f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(280.109f, 8.f + m_fPointY, 318.153f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(288.109f, 8.f + m_fPointY, 317.753f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(296.109f, 8.f + m_fPointY, 317.353f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(304.109f, 8.f + m_fPointY, 316.953f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(256.161f, 5.f + m_fPointY, 319.372f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(264.109f, 5.f + m_fPointY, 318.953f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(272.109f, 2.f + m_fPointY, 318.553f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(248.109f, 2.f + m_fPointY, 319.773f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(280.109f, 0.f + m_fPointY, 318.153f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(288.109f, 0.f + m_fPointY, 317.753f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(296.109f, 0.f + m_fPointY, 317.353f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(304.109f, 0.f + m_fPointY, 316.953f), m_fPointRadius, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));	
	m_pLightMgr->AddPointLight(D3DXVECTOR3(259.48f, 18.2222f + m_fPointY, 142.924f), 3.f, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(259.48f, 20.555f + m_fPointY, 140.524f), 3.f, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(259.48f, 18.2222f + m_fPointY, 137.524f), 3.f, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(257.48f, 18.2222f + m_fPointY, 140.866f), 3.f, D3DXVECTOR3(0.f, 0.f, m_fPointColor[0]));

	// 노랑
	m_pLightMgr->AddPointLight(D3DXVECTOR3(145.844f, 0.f + m_fPointY, 405.368f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(156.513f, 0.f + m_fPointY, 403.979f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(156.798f, 0.f + m_fPointY, 393.433f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(264.912f, 18.f + m_fPointY, 367.82f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.683f, 18.f + m_fPointY, 359.509f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.164f, 18.f + m_fPointY, 351.829f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.839f, 18.f + m_fPointY, 344.844f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.085f, 18.f + m_fPointY, 338.866f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(253.751f, 18.f + m_fPointY, 365.297f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(252.876f, 18.f + m_fPointY, 359.522f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(252.955f, 18.f + m_fPointY, 352.078f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(252.01f, 18.f + m_fPointY, 345.47f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(251.769f, 18.f + m_fPointY, 339.204f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(252.01f, 18.f + m_fPointY, 332.87f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.404f, 18.f + m_fPointY, 333.864f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.649f, 18.1111f + m_fPointY, 304.897f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(253.479f, 18.1111f + m_fPointY, 304.006f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.606f, 18.1111f + m_fPointY, 294.274f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.04f, 18.1067f + m_fPointY, 289.976f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.517f, 18.f + m_fPointY, 280.742f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.264f, 18.f + m_fPointY, 273.96f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.028f, 18.f + m_fPointY, 266.355f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.824f, 18.1111f + m_fPointY, 259.641f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.844f, 18.1111f + m_fPointY, 254.628f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.841f, 18.1111f + m_fPointY, 244.274f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.807f, 18.1111f + m_fPointY, 234.039f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.392f, 18.1111f + m_fPointY, 227.941f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.891f, 18.1111f + m_fPointY, 221.781f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.95f, 18.1111f + m_fPointY, 215.221f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.934f, 18.1111f + m_fPointY, 206.293f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.962f, 18.1111f + m_fPointY, 198.876f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.919f, 18.1111f + m_fPointY, 191.786f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.934f, 18.1111f + m_fPointY, 186.923f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.757f, 18.1111f + m_fPointY, 181.877f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.737f, 18.1111f + m_fPointY, 177.18f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.907f, 18.1111f + m_fPointY, 172.433f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.786f, 18.1111f + m_fPointY, 165.822f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(265.872f, 18.1111f + m_fPointY, 159.046f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(266.378f, 18.1111f + m_fPointY, 152.02f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.166f, 18.f + m_fPointY, 278.764f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.205f, 18.f + m_fPointY, 272.958f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(252.166f, 18.1111f + m_fPointY, 294.723f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.205f, 18.1111f + m_fPointY, 288.475f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.194f, 18.f + m_fPointY, 266.769f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.058f, 18.1111f + m_fPointY, 260.079f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.02f, 18.1111f + m_fPointY, 254.627f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.091f, 18.1111f + m_fPointY, 245.008f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.154f, 18.1111f + m_fPointY, 234.882f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.112f, 18.1111f + m_fPointY, 228.737f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.116f, 18.1111f + m_fPointY, 222.345f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.182f, 18.1111f + m_fPointY, 215.375f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.248f, 18.1111f + m_fPointY, 205.7f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.27f, 18.1111f + m_fPointY, 198.927f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.25f, 18.1111f + m_fPointY, 193.31f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.159f, 18.1111f + m_fPointY, 187.653f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.192f, 18.1111f + m_fPointY, 182.47f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.25f, 18.1111f + m_fPointY, 177.508f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.175f, 18.1111f + m_fPointY, 171.11f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.173f, 18.1111f + m_fPointY, 164.089f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(254.177f, 18.1111f + m_fPointY, 157.071f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
	m_pLightMgr->AddPointLight(D3DXVECTOR3(253.679f, 18.1111f + m_fPointY, 150.711f), m_fPointRadius, D3DXVECTOR3(m_fPointColor[RGB_RED], m_fPointColor[RGB_GREEN], m_fPointColor[RGB_BLUE]));
}

void CRenderMgr::Render(const float & fTime)
{
	// 키세팅
	Input(fTime);

	if (m_bDefferdOn)
	{
		// 디렉셔널 갱신
		m_pLightMgr->SetDirectional(
			D3DXVECTOR3(m_vDirLight.x, m_vDirLight.y, m_vDirLight.z),
			D3DXVECTOR3(m_fDirColor[RGB_RED], m_fDirColor[RGB_GREEN], m_fDirColor[RGB_BLUE]));

		InitPointLight();
	}

	if (m_bDefferdOn)
		Render_ShadowMap();

	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	m_pDevice->m_pDeviceContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set render resources
	//m_pDevice->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSampLinear);

	m_pTargetMgr->GetGBuffer()->Begin_MRT(m_pDevice->m_pDeviceContext);

	// GBuffer	
	if (m_bDefferdOn)
	{
		Render_Priority();
		Render_NoneAlpha();
	}

	m_pTargetMgr->GetGBuffer()->End_MRT(m_pDevice->m_pDeviceContext);

	if (m_bDefferdOn)
		Render_BorderLine();

	// Set the render target and do the lighting
	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthReadOnlyDSV());

	m_pTargetMgr->GetGBuffer()->PrepareForUnpack(m_pDevice->m_pDeviceContext);
	m_pLightMgr->DoLighting(m_pDevice->m_pDeviceContext, m_pTargetMgr->GetGBuffer(), m_pTargetMgr->GetBorderMRT());

	if (m_bRenderGBuffer)
	{
		m_pLightMgr->DoDebugLightVolume(m_pDevice->m_pDeviceContext);
		m_pTargetMgr->RenderGBuffer(m_pDevice->m_pDeviceContext);
	}

	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthDSV());

	m_pDevice->m_pDeviceContext->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	Safe_Release(pPrevDepthState);

	if (!m_bDefferdOn)
	{
		Render_Priority();
		Render_NoneAlpha();
	}

	Render_Alpha();

	if (!m_bRenderGBuffer)
		Render_UI();

	Render_FPS(fTime);
}

void CRenderMgr::Render_Priority(void)
{
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_PRIORITY].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_PRIORITY].end();

	for (; iter != iter_end;)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_PRIORITY].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
}

void CRenderMgr::Render_NoneAlpha(void)
{
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_NONEALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_NONEALPHA].end();

	for (; iter != iter_end;)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_NONEALPHA].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
}

bool Compare_ViewZ(CObj* pSour, CObj* pDest)
{
	return pSour->GetViewZ() > pDest->GetViewZ();
}

void CRenderMgr::Render_Alpha(void)
{
	m_RenderGroup[TYPE_ALPHA].sort(Compare_ViewZ);

	// 	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// 	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// 
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_ALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_ALPHA].end();

	for (; iter != iter_end; iter)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_ALPHA].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
}

void CRenderMgr::Render_UI(void)
{
	m_pDevice->Blend_Begin();

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_UI].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_UI].end();

	for (; iter != iter_end; iter)
	{
		if ((*iter) == NULL)
			m_RenderGroup[TYPE_UI].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
	m_pDevice->Blend_End();
}

void CRenderMgr::Render_FPS(const float & fTime)
{
	m_fTime += fTime;
	++m_dwCount;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFps, L"FPS : %d", m_dwCount);
		m_fTime = 0.f;
		m_dwCount = 0;
	}
	m_pFPSFont->m_wstrText = m_szFps;

	m_pFPSFont->Render();


}

void CRenderMgr::Render_BorderLine(void)
{
	ID3D11DeviceContext* pd3dImmediateContext = m_pDevice->m_pDeviceContext;

	// Fill the GBuffer unpack constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// 상수 버퍼의 내용을 쓸 수 있도록 잠금
	pd3dImmediateContext->Map(m_pLinearDepthCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	// 상수 버퍼의 데이터에 대한 포인터를 가져옴
	CB_LINEAR_DEPTH* pGBufferUnpackCB = (CB_LINEAR_DEPTH*)MappedResource.pData;
	const D3DXMATRIX* pProj = CCamera::GetInstance()->GetProjMatrix();

	// 상수 버퍼에 값을 복사
	pGBufferUnpackCB->PerspectiveValues.x = m_fSobelValue;
	pGBufferUnpackCB->PerspectiveValues.y = 1.0f / pProj->m[1][1];
	pGBufferUnpackCB->PerspectiveValues.z = pProj->m[3][2];
	pGBufferUnpackCB->PerspectiveValues.w = -pProj->m[2][2];

	// 상수 버퍼의 잠금을 품
	pd3dImmediateContext->Unmap(m_pLinearDepthCB, 0);

	// 픽셀 셰이더의 상수 버퍼를 바뀐 값으로 바꿈
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &m_pLinearDepthCB);

	m_pTargetMgr->GetBorderMRT()->Begin_MRT(pd3dImmediateContext);

	pd3dImmediateContext->IASetInputLayout(m_pBorderLineVS->m_pVertexLayout);

	pd3dImmediateContext->VSSetShader(m_pBorderLineVS->m_pVertexShader, NULL, 0);

	pd3dImmediateContext->PSSetShader(m_pBorderLinePS->m_pPixelShader, NULL, 0);
	ID3D11ShaderResourceView* pDepthSRV = m_pTargetMgr->GetGBuffer()->GetDepthView();
	pd3dImmediateContext->PSSetShaderResources(0, 1, &pDepthSRV);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	m_pRcTex->Render();

	m_pTargetMgr->GetBorderMRT()->End_MRT(pd3dImmediateContext);
}

void CRenderMgr::Render_ShadowMap(void)
{
	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	m_pDevice->m_pDeviceContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	//Prepare for Render Shadow Maps
	D3D11_VIEWPORT oldvp;
	UINT num = 1;
	m_pDevice->m_pDeviceContext->RSGetViewports(&num, &oldvp);
	ID3D11RasterizerState* pPrevRSState;
	m_pDevice->m_pDeviceContext->RSGetState(&pPrevRSState);

	m_pLightMgr->PrepareNextShadowLight(m_pDevice->m_pDeviceContext);

	// Generate the shadow maps

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_NONEALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_NONEALPHA].end();

	for (; iter != iter_end;)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_NONEALPHA].erase(iter);
		else
		{
			(*iter)->ShadowmapRender();
			++iter;
		}
	}

	// Restore the states
	m_pDevice->m_pDeviceContext->RSSetViewports(num, &oldvp);
	m_pDevice->m_pDeviceContext->RSSetState(pPrevRSState);
	Safe_Release(pPrevRSState);

	// Cleanup
	m_pDevice->m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	m_pDevice->m_pDeviceContext->GSSetShader(NULL, NULL, 0);

	m_pDevice->m_pDeviceContext->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	Safe_Release(pPrevDepthState);
}

void CRenderMgr::Release(void)
{
}

void CRenderMgr::ListClear(void)
{
	for (UINT i = 0; i < TYPE_END; ++i)
	{
		m_RenderGroup[i].clear();
	}
}

void CRenderMgr::Input(float fTime)
{
	m_fTimeCheck += fTime;

	if (m_fTimeCheck >= 0.5f)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_TAB) & 0x80)
		{
			if (m_bRenderGBuffer)
				m_bRenderGBuffer = false;
			else
				m_bRenderGBuffer = true;
			m_fTimeCheck = 0.f;
		}
	}

	if (m_fTimeCheck >= 0.5f)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_0) & 0x80)
		{
			if (m_bDefferdOn)
				m_bDefferdOn = false;
			else
				m_bDefferdOn = true;
			m_fTimeCheck = 0.f;
		}
	}

	/*if (CInput::GetInstance()->GetDIKeyState(DIK_C) & 0x80)
	{
		m_fPointY += 0.1f;
	cout << "PointY : " << m_fPointY << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_V) & 0x80)
	{
		m_fPointY -= 0.1f;
	cout << "PointY : " << m_fPointY << endl;
	}*/

	/*if (CInput::GetInstance()->GetDIKeyState(DIK_B) & 0x80)
	{
		m_fPointRadius += 0.1f;
		cout << "PointRadius : " << m_fPointRadius << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_N) & 0x80)
	{
		m_fPointRadius -= 0.1f;
		cout << "PointRadius : " << m_fPointRadius << endl;
	}*/

	//if (CInput::GetInstance()->GetDIKeyState(DIK_Z) & 0x80)
	//{
	//	m_fPointColor[0] += 0.1f;
	//	//m_fPointColor[1] += 0.1f;
	//	cout << "m_fPointColor : " << m_fPointColor[0] << endl;
	//}

	//if (CInput::GetInstance()->GetDIKeyState(DIK_X) & 0x80)
	//{
	//	m_fPointColor[0] -= 0.1f;
	//	//m_fPointColor[1] -= 0.1f;
	//	cout << "m_fPointColor : " << m_fPointColor[0] << endl;
	//}

	/*if (CInput::GetInstance()->GetDIKeyState(DIK_Z) & 0x80)
	{
		m_fSobelValue += 0.0001f;
		cout << "SobelValue : " << m_fSobelValue << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_X) & 0x80)
	{
		m_fSobelValue -= 0.0001f;
		cout << "SobelValue : " << m_fSobelValue << endl;
	}*/

	// For.Red
	//if (CInput::GetInstance()->GetDIKeyState(DIK_R) & 0x80)
	//{
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	//	{
	//		if (1.0f <= m_fDirColor[RGB_RED])
	//			m_fDirColor[RGB_RED] = 1.0f;
	//		else
	//			m_fDirColor[RGB_RED] += 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	//	{
	//		if (0.0f >= m_fDirColor[RGB_RED])
	//			m_fDirColor[RGB_RED] = 0.0f;
	//		else
	//			m_fDirColor[RGB_RED] -= 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//}

	//// For.Green
	//if (CInput::GetInstance()->GetDIKeyState(DIK_G) & 0x80)
	//{
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	//	{
	//		if (1.0f <= m_fDirColor[RGB_GREEN])
	//			m_fDirColor[RGB_GREEN] = 1.0f;
	//		else
	//			m_fDirColor[RGB_GREEN] += 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	//	{
	//		if (0.0f >= m_fDirColor[RGB_GREEN])
	//			m_fDirColor[RGB_GREEN] = 0.0f;
	//		else
	//			m_fDirColor[RGB_GREEN] -= 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//}

	//// For.Blue
	//if (CInput::GetInstance()->GetDIKeyState(DIK_B) & 0x80)
	//{
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	//	{
	//		if (1.0f <= m_fDirColor[RGB_BLUE])
	//			m_fDirColor[RGB_BLUE] = 1.0f;
	//		else
	//			m_fDirColor[RGB_BLUE] += 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	//	{
	//		if (0.0f >= m_fDirColor[RGB_BLUE])
	//			m_fDirColor[RGB_BLUE] = 0.0f;
	//		else
	//			m_fDirColor[RGB_BLUE] -= 0.1f * fTime;

	//		cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
	//	}
	//}

	/*if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RBUTTON))
	{

		float fDX = (float)CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X);
		float fDY = (float)CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y);

		D3DXMATRIX viewInvMat;
		D3DXMatrixInverse(&viewInvMat, NULL, CCamera::GetInstance()->GetViewMatrix());
		D3DXVECTOR3 vRight = D3DXVECTOR3(viewInvMat._11, viewInvMat._12, viewInvMat._13);
		D3DXVECTOR3 vUp = D3DXVECTOR3(viewInvMat._21, viewInvMat._22, viewInvMat._23);
		D3DXVECTOR3 vForward = D3DXVECTOR3(viewInvMat._31, viewInvMat._32, viewInvMat._33);

		m_vDirLight -= vRight * fDX * fTime;
		m_vDirLight -= vUp * fDY * fTime;
		m_vDirLight += vForward * fDY * fTime;

		D3DXVec3Normalize(&m_vDirLight, &m_vDirLight);

		cout << "m_vDirLight.x : " << m_vDirLight.x << " " << "m_vDirLight.y : " << m_vDirLight.y << " " << "m_vDirLight.z : " << m_vDirLight.z << endl;
	}*/
}