
// MapToolView.cpp : CMapToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MapTool.h"
#endif

#include "MapToolDoc.h"
#include "MapToolView.h"
#include "Device.h"
#include "Include.h"
#include "MainFrm.h"
#include "TimeMgr.h"
#include "Camera.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Input.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "Back.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "ParsingDevice9.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapToolView

IMPLEMENT_DYNCREATE(CMapToolView, CView)

BEGIN_MESSAGE_MAP(CMapToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMapToolView 생성/소멸

CMapToolView::CMapToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMapToolView::~CMapToolView()
{
}

BOOL CMapToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMapToolView 그리기

void CMapToolView::OnDraw(CDC* /*pDC*/)
{
	CMapToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMapToolView 인쇄

BOOL CMapToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMapToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMapToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CMapToolView 진단

#ifdef _DEBUG
void CMapToolView::AssertValid() const
{
	CView::AssertValid();
}

void CMapToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CMapToolView::SetCurToolIndex(int& iIndex)
{
	m_iCurToolIndex = iIndex;
}

CMapToolDoc* CMapToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapToolDoc)));
	return (CMapToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapToolView 메시지 처리기


void CMapToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CMainFrame*	pFrame = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWindow;
	pFrame->GetWindowRect(&rcWindow);

	SetRect(&rcWindow, 0, 0, rcWindow.right - rcWindow.left,
		rcWindow.bottom - rcWindow.top);

	RECT rcMainView;
	GetClientRect(&rcMainView);

	float fRowFrm = float(rcWindow.right - rcMainView.right);
	float fColFrm = float(rcWindow.bottom - rcMainView.bottom);


	pFrame->SetWindowPos(NULL, 0, 0,
		int(WINCX + fRowFrm),
		int(WINCY + fColFrm), SWP_NOZORDER);

	g_hWnd = m_hWnd;

	CDevice::GetInstance()->CreateDevice();

	CParsingDevice9::GetInstance()->InitGraphicDev(CParsingDevice9::MODE_WIN, g_hWnd, WINCX, WINCY);


	CTimeMgr::GetInstance()->InitTime();

	if (FAILED(CCamera::GetInstance()->Initialize()))
	{
		AfxMessageBox(L"Camera Initialize Failed", MB_OK);
	}



	if (FAILED(CResourcesMgr::GetInstance()->ReserveContainerSize(RESOURCE_END)))
	{
		AfxMessageBox(L"Resource Container Reserve Failed", MB_OK);
		return;
	}

	//일반

	if (CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"Shader.fx", "VS", "vs_5_0", SHADER_VS))
	{
		AfxMessageBox(L"Vertex Shader(Default) Create Failed", MB_OK);
		return;
	}


	if (CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"Shader.fx", "PS", "ps_5_0", SHADER_PS))
	{
		AfxMessageBox(L"PIXEL Shader(Default) Create Failed", MB_OK);
		return;
	}
	//다이나믹 매쉬용
	if (CShaderMgr::GetInstance()->AddShaderFiles(L"VS_ANI", L"Shader.fx", "VS_ANI", "vs_5_0", SHADER_ANI))
	{
		AfxMessageBox(L"Vertex Shader(Ani) Create Failed", MB_OK);
		return;
	}

	//네비메쉬 라인용

	/*if (CShaderMgr::GetInstance()->AddShaderFiles(L"VS_LINE", L"LineShader.fx", "VS_LINE", "vs_5_0", SHADER_LINE_VS))
	{
		AfxMessageBox(L"Vertex Shader(Line) Create Failed", MB_OK);
		return;
	}


	if (CShaderMgr::GetInstance()->AddShaderFiles(L"PS_LINE", L"LineShader.fx", "PS_LINE", "ps_5_0", SHADER_LINE_PS))
	{
		AfxMessageBox(L"PIXEL Shader(Line) Create Failed", MB_OK);
		return;
	}*/

	if (CInput::GetInstance()->InitInputDevice(AfxGetInstanceHandle(), g_hWnd))
	{
		AfxMessageBox(L"DInput Create Failed", MB_OK);
		return;
	}

	CScene* pScene = NULL;
	pScene = CBack::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_BACK, pScene);

	CSceneMgr::GetInstance()->ChangeScene(SCENE_BACK);



	SetTimer(1, 10, NULL);

	m_iCurToolIndex = 0;
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void CMapToolView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnTimer(nIDEvent);

	CTimeMgr::GetInstance()->SetTime();
	CInput::GetInstance()->SetInputState();
	CCamera::GetInstance()->Update();

	CSceneMgr::GetInstance()->Update();


	CDevice::GetInstance()->BeginDevice();
	CSceneMgr::GetInstance()->Render();
	CDevice::GetInstance()->EndDevice();
}


void CMapToolView::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDevice::GetInstance()->DestroyInstance();
	CParsingDevice9::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CCamera::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();

	CView::PostNcDestroy();
}


void CMapToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/o또는 기본값을 호출합니다.

	switch (m_iCurToolIndex)
	{
	case 0:
		((CBack*)CSceneMgr::GetInstance()->GetScene())->ConstObjectMode();
		break;
	case 1:
		break;
	case 2:
		break;
	}

	CView::OnLButtonDown(nFlags, point);
}
