#include "stdafx.h"
#include "MainApp.h"

//½Ì±ÛÅæ °´Ã¼
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

//ÀÏ¹Ý°´Ã¼
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

	CDevice::GetInstance()->CreateDevice();
	m_pGrapicDevcie = CDevice::GetInstance();

	CParsingDevice9::GetInstance()->InitGraphicDev(CParsingDevice9::MODE_WIN, g_hWnd, WINCX, WINCY);

	CTimeMgr::GetInstance()->InitTime();

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


	//ÀÏ¹Ý
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

	//·Î°í¿ë
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_Logo", L"../ShaderCode/Shader.fx", "VS_Logo", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(LOGO) Create Failed", MB_OK);
		return hr;
	}
	/////////////////////////////////////////////////////////////////////////

	//´ÙÀÌ³ª¹Í ¸Å½¬¿ë
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_ANI", L"../ShaderCode/Shader.fx", "VS_ANI", "vs_5_0", SHADER_ANI);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Ani) Create Failed", MB_OK);
		return hr;
	}
	////////////////////////////////////////////////////////////////////////

	hr = CInput::GetInstance()->InitInputDevice(g_hInst, g_hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DInput Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	hr = CResourcesMgr::GetInstance()->AddBuffer(RESOURCE_STATIC, BUFFER_RCTEX, L"Buffer_RcTex");
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"Buffer_RcTex »ý¼º ½ÇÆÐ");

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
}
