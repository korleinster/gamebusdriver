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
#include "ParsingDevice9.h"
#include "AnimationMgr.h"
#include "Frustum.h"

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

	//_CrtSetBreakAlloc(280);
}


CMainApp::~CMainApp()
{
	FreeConsole();
	Release();
}

HRESULT CMainApp::Initialize(void)
{
	srand(UINT(time(NULL)));

	CDevice::GetInstance()->CreateDevice();
	m_pGrapicDevcie = CDevice::GetInstance();

	CParsingDevice9::GetInstance()->InitGraphicDev(CParsingDevice9::MODE_WIN, g_hWnd, WINCX, WINCY);

	CTimeMgr::GetInstance()->InitTime();

	if (FAILED(CCamera::GetInstance()->Initialize()))
	{
		MessageBox(NULL, L"System Message", L"Camera Initialize Failed", MB_OK);
	}
	
	HRESULT hr = S_OK;

	hr = CResourcesMgr::GetInstance()->AddBuffer(RESOURCE_STATIC, BUFFER_RCTEX, L"Buffer_RcTex");
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"Buffer_RcTex 생성 실패");

	hr = CResourcesMgr::GetInstance()->AddBuffer(RESOURCE_LOGO, BUFFER_RCTEX, L"Buffer_RcLogo");
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"Buffer_RcLogo 생성 실패");


	//// Real 디퍼드용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"DebugBufferVS", L"../ShaderCode/DebugBuffer.fx", "DebugBufferVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DebugBuffer Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"DebugBufferPS", L"../ShaderCode/DebugBuffer.fx", "DebugBufferPS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DebugBuffer Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"DebugBufferPS_Red", L"../ShaderCode/DebugBuffer.fx", "DebugBufferPS_Red", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DebugBuffer_Red Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	//// 윤곽선
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"BorderLineVS", L"../ShaderCode/BorderLine.fx", "BorderLineVS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DebugBuffer Vertex Shader Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"BorderLinePS", L"../ShaderCode/BorderLine.fx", "BorderLinePS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"DebugBuffer Pixel Shader Create Failed", MB_OK);
		return hr;
	}

	//일반
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"../ShaderCode/Shader.fx", "VS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Default) Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"../ShaderCode/Shader.fx", "PS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PIXEL Shader(Default) Create Failed", MB_OK);
		return hr;
	}
	//////////////////////////////////////////////////////////////////////////

	//로고용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_Logo", L"../ShaderCode/Shader.fx", "VS_Logo", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(LOGO) Create Failed", MB_OK);
		return hr;
	}
	/////////////////////////////////////////////////////////////////////////

	//다이나믹 매쉬용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_ANI", L"../ShaderCode/Shader.fx", "VS_ANI", "vs_5_0", SHADER_ANI);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Ani) Create Failed", MB_OK);
		return hr;
	}

	//// 디퍼드용
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

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"RenderSceneVS_ANI", L"../ShaderCode/DeferredShading.fx", "RenderSceneVS_ANI", "vs_5_0", SHADER_ANI);
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
	////////////////////////////////////////////////////////////////////////

	hr = CInput::GetInstance()->InitInputDevice(g_hInst, g_hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DInput Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	CScene* pScene = NULL;

	pScene = CLogo::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_LOGO, pScene);

	/*pScene = CStage::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);*/

	CSceneMgr::GetInstance()->ChangeScene(SCENE_LOGO);

	/*D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;*/
			
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
	CParsingDevice9::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CCamera::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();
	CResourcesMgr::GetInstance()->DestroyInstance();
	CAnimationMgr::GetInstance()->DestroyInstance();
}
