// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//



#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"
#include "Include.h"
#include "ChatUI.h"
#include "ObjMgr.h"
#include "Font.h"
//#include <vld.h>

// 전역 변수:
HINSTANCE g_hInst; // 현재 인스턴스입니다.

// ClientClass.cpp - Init 함수에서 스크립트 값을 읽어온다.
int MAX_AI_NUM;
int MAX_AI_SLIME;
int MAX_AI_GOBLIN;
int MAX_AI_BOSS;

TCHAR cText[MAX_CHAT_SIZE / 2];//저장할 채팅 메시지
TCHAR cCompText[10];//조합중인 문자
TCHAR cCanText[200];//특수문자
int iCNum = 0;//특수문자 위치
int iCMax = 0;//특수문자 목록 최대갯수

HWND	g_hWnd;
DWORD	g_dwLightIndex = 0;
D3DXVECTOR3 g_vLightDir = D3DXVECTOR3(1.f, -1.f, 1.f);
AsynchronousClientClass g_client;
bool g_bLogin = false;
bool g_bChatMode = false; 
bool g_bChatEnd = true;
float g_fChatCool = 0.f;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int GetText(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_wsetlocale(LC_ALL, L"korean");

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"노루막이";
	if (false == RegisterClass(&wndclass)) { return -1; }

	// 윈도우 생성
	RECT rc = { 0,0,WINCX, WINCY };
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	HWND hWnd = CreateWindow(L"노루막이", L"노루막이", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	g_hWnd = hWnd;
	if (NULL == hWnd) { return -1; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef _DEBUG
	// 이거 지우지 않으면 release 모드에서 문제생길 확률이 매우 높다.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG



	HACCEL hAccelTable = LoadAccelerators(hInstance, 0);

	MSG msg;
	msg.message = WM_NULL;

	CMainApp*		pMainApp = CMainApp::Create();

	ZeroMemory(&cText, sizeof(char) * MAX_BUF_SIZE);
	ZeroMemory(&cCompText, sizeof(char) * 10);

	// 기본 메시지 루프입니다.
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			if (g_bChatMode == true)
			{
				if (_tcslen(cText) == 0)
				{
					g_bChatEnd = true;

				}
				else
				{
					g_bChatEnd = false;

				}
				CChatUI* pChatUI = dynamic_cast<CChatUI*>(*(CObjMgr::GetInstance()->Get_ObjList(L"ChatUI")->begin()));

				pChatUI->m_pFont->m_wstrText = cText;

				if (GetAsyncKeyState(VK_RETURN) && g_bChatEnd == false)
				{
					//상대방에게 메시지를 보내는 부분

					// 여기다가 데이터를 담은 뒤 전송
					g_client.sendPacket(MAX_CHAT_SIZE / 2, CHAT, reinterpret_cast<Packet*>(&cText));

					ZeroMemory(&cText, sizeof(TCHAR) * (MAX_CHAT_SIZE / 2));
					//보내고나서 문자열을 비워준다.
					g_fChatCool = 0.f;
				}
			}
			pMainApp->Update();
			pMainApp->Render();
		}
	}

	::Safe_Delete(pMainApp);

	return (int)msg.wParam;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	switch (iMessage)
	{
	case WM_INITDIALOG: {

		SetDlgItemText(hDlg, IDC_IP, L"127.0.0.1");
		SetDlgItemText(hDlg, IDC_ID, L"guest");
		SetDlgItemText(hDlg, IDC_PW, L"guest");

		return TRUE;
	}
	case WM_COMMAND: {

		switch (LOWORD(wParam))
		{
		case IDOK:
			wchar_t ip[32];

			GetDlgItemText(hDlg, IDC_IP, ip, 32);
			WideCharToMultiByte(CP_ACP, 0, ip, -1, g_client.get_server_ip_array(), 32, 0, 0);

			GetDlgItemText(hDlg, IDC_ID, g_client.get_login_id_array(), MAX_BUF_SIZE / 4);
			GetDlgItemText(hDlg, IDC_PW, g_client.get_login_pw_array(), MAX_BUF_SIZE / 4);

			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			PostQuitMessage(0);
			return TRUE;
		default:
			break;
		}

		break;
	}
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	if (g_bChatMode == true)
	{
		if (GetText(hWnd, message, wParam, lParam) == 0)
			return 0;
	}


	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SOCKET:
		g_client.ProcessWinMessage(hWnd, message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int GetText(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iLen;
	HIMC hIMC = NULL;//ime 핸들

	switch (msg)
	{
	case WM_IME_COMPOSITION://한글 조합
		hIMC = ImmGetContext(hWnd);
		if (lParam & GCS_RESULTSTR)
		{
			if ((iLen = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0)) > 0)// ime의 스트링 길이를 받아옴
			{


				ImmGetCompositionString(hIMC, GCS_RESULTSTR, cCompText, iLen);
				//조합중인 문자열을 받아옴.

				cCompText[iLen] = 0;
				//끝에 0을 넣어 마무리.

				wcout << L"조합출력1:" << cCompText << endl;

				_tcscpy(cText + _tcslen(cText), cCompText);
				//전체 메시지 뒤에 붙여줌.

				ZeroMemory(&cCompText, sizeof(char) * 10);
				//조합중인 문자열 초기화
			}
		}
		else if (lParam & GCS_COMPSTR) //조합중이면
		{

			iLen = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
			// 조합중인 길이를 얻는다.

			ImmGetCompositionString(hIMC, GCS_COMPSTR, cCompText, iLen);
			//조합중인 문자를 얻는다.

			cCompText[iLen] = 0;
			//역시 마무리

			wcout << L"조합출력2:" << *cCompText << endl;

		}
		ImmReleaseContext(hWnd, hIMC);
		return 0;
	case WM_CHAR://영어랑 백스페이스 등등
		if (wParam == CHAT_BACKSPACE)
		{
			if (_tcslen(cText) > 0) // 내용이 있으면
			{
				if (_tcslen(cText) < 0)//지울게 확장코드이면
				{
					cText[_tcslen(cText) - 1] = 0; //한자를 지움
				}

				cText[_tcslen(cText) - 1] = 0;//한자를 지운다

				ZeroMemory(&cCompText, sizeof(char) * 10); // 조합중인 문자 초기화
			}
		}
		else if (wParam == CHAT_ENTER)
		{
			//엔터예외처리
		}
		else
		{
			iLen = _tcslen(cText);
			cText[iLen] = wParam & 0xff; // 넘어온 문자를 넣기
			//cText[iLen] = 0;//역시 마무리
		}
		return 0;
	case WM_IME_NOTIFY://한자
					   //GetCandi(hWnd, wParam, lParam, 0);
		return 0;
		//case WM_KEYDOWN://키다운
		//return 0;
	default:
		break;
	}
}

//void GetCandi(HWND hWnd, WPARAM wParam, LPARAM lParam, int Number)
//{
//	//특문과 한자는 나중에
//}

