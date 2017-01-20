#include "stdafx.h"
#include "MainApp.h"

//싱글톤 객체
#include "Device.h"
#include "TimeMgr.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Input.h"
#include "Camera.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "SceneMgr.h"

//일반객체
#include "Shader.h"
#include "Obj.h"
#include "RcCol.h"
#include "Info.h"
#include "CubeCol.h"
#include "RcTex.h"
#include "Texture.h"
#include "RcTerrain.h"
#include "Scene.h"
#include "Logo.h"
#include "Stage.h"


CMainApp::CMainApp()
	: m_pRcCol(NULL),
	m_pInfo(NULL)
{
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	SetConsoleTitleA("Debug");
}


CMainApp::~CMainApp()
{
	FreeConsole();
	Release();

}

HRESULT CMainApp::Initialize(void)
{
	srand(UINT(time(NULL)));

	m_pInfo = CInfo::Create(D3DXVECTOR3(0,0,1));

	CDevice::GetInstance()->CreateDevice();
	m_pGrapicDevcie = CDevice::GetInstance();

	CTimeMgr::GetInstance()->InitTime();

	
	//m_pRcCol = CRcTerrain::Create(VERTEXCOUNTX, VERTEXCOUNTZ, VERTEXINTERVAL);
	//m_pRcCol = CRcTex::Create();

	if (FAILED(CCamera::GetInstance()->Initialize()))
	{
		MessageBox(NULL, L"System Message", L"Camera Initialize Failed", MB_OK);
	}
	
	HRESULT hr = S_OK;

	if (FAILED(CResourcesMgr::GetInstance()->ReserveContainerSize(RESOURCE_END)))
	{
		MessageBox(NULL, L"System Message", L"Resource Container Reserve Failed", MB_OK);
		return E_FAIL;
	}


	// ShaderMgr 사용법
	// 셰이더를 탐색할 Tag, 파일 경로, 진입함수, 버전, 셰이더종류
	// 일반
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"../ShaderCode/Shader.fx", "VS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"../ShaderCode/Shader.fx", "PS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	// 로고용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_Logo", L"../ShaderCode/Shader.fx", "VS_Logo", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	// 디퍼드용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"GBufferVisVS", L"../ShaderCode/GBufferVis.fx", "GBufferVisVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"GBuffer Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"GBufferVisPS", L"../ShaderCode/GBufferVis.fx", "GBufferVisPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"GBuffer Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	// 디퍼드 오브젝트용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"RenderSceneVS", L"../ShaderCode/DeferredShading.fx", "RenderSceneVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Render Scene Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"RenderScenePS", L"../ShaderCode/DeferredShading.fx", "RenderScenePS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Render Scene Pixel Shader Create Failed", MB_OK);
		return hr;
	}
	
	// 조명 쉐이더
	// Directional
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"DirLightVS", L"../ShaderCode/DirLight.fx", "DirLightVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DirLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"DirLightPS", L"../ShaderCode/DirLight.fx", "DirLightPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DirLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	// Point
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PointLightVS", L"../ShaderCode/PointLight.fx", "PointLightVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PointLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PointLightHS", L"../ShaderCode/PointLight.fx", "PointLightHS", "hs_5_0", SHADER_HS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PointLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PointLightDS", L"../ShaderCode/PointLight.fx", "PointLightDS", "ds_5_0", SHADER_DS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PointLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PointLightPS", L"../ShaderCode/PointLight.fx", "PointLightPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PointLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	// Spot
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"SpotLightVS", L"../ShaderCode/SpotLight.fx", "SpotLightVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"SpotLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"SpotLightHS", L"../ShaderCode/SpotLight.fx", "SpotLightHS", "hs_5_0", SHADER_HS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"SpotLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"SpotLightDS", L"../ShaderCode/SpotLight.fx", "SpotLightDS", "ds_5_0", SHADER_DS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"SpotLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"SpotLightPS", L"../ShaderCode/SpotLight.fx", "SpotLightPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"SpotLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}
	// Capsule
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"CapsuleLightVS", L"../ShaderCode/CapsuleLight.fx", "CapsuleLightVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Capsule Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"CapsuleLightHS", L"../ShaderCode/CapsuleLight.fx", "CapsuleLightHS", "hs_5_0", SHADER_HS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Capsule Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"CapsuleLightDS", L"../ShaderCode/CapsuleLight.fx", "CapsuleLightDS", "ds_5_0", SHADER_DS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"CapsuleLight Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"CapsuleLightPS", L"../ShaderCode/CapsuleLight.fx", "CapsuleLightPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"CapsuleLight Pixel Shader Create Failed", MB_OK);
		return hr;
	}


	// 인풋
	hr = CInput::GetInstance()->InitInputDevice(g_hInst, g_hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DInput Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	CScene* pScene = NULL;

	pScene = CLogo::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_LOGO, pScene);

	pScene = CStage::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);

	CSceneMgr::GetInstance()->ChangeScene(SCENE_LOGO);	
			
	return S_OK;
}

int CMainApp::Update(void)
{
	CTimeMgr::GetInstance()->SetTime();
	CInput::GetInstance()->SetInputState();
	CCamera::GetInstance()->Update();

	CSceneMgr::GetInstance()->Update();


	return 0;
}

void CMainApp::Render(void)
{
	m_pGrapicDevcie->BeginDevice();
	CSceneMgr::GetInstance()->Render();
	m_pGrapicDevcie->EndDevice();
}

CMainApp * CMainApp::Create(void)
{
	CMainApp*		pMainApp = new CMainApp;
	if (FAILED(pMainApp->Initialize()))
	{
		delete pMainApp;
		pMainApp = NULL;
	}
	return pMainApp;
}

void CMainApp::Release(void)
{
	CDevice::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CCamera::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();
}
