#include"stdafx.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 그리기 위한 변수 - WM_PAINT
int view_range{ 150 };
int value{ 10 };
// ------------------------------


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
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
	wndclass.lpszClassName = L"MyWndClass";
	if (false == RegisterClass(&wndclass)) { return -1; }

	// 윈도우 생성
	HWND hWnd = CreateWindow(L"MyWndClass", L"WinApp", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
	if (NULL == hWnd) { return -1; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 서버와의 통신 진행 - 디버그 모드가 아니라면 ServerIP.txt 파일에 적힌 ip 주소로 바로 연결이 된다.
	g_client.Init(hWnd);

	// 메시지 루프
	MSG msg;
	/*while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	HACCEL hAccelTable = LoadAccelerators(hInstance, 0);

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			
		}
	}

	return msg.wParam;
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// 그리기 위한 변수 - WM_PAINT
	HDC hdc;
	PAINTSTRUCT ps;
	// ------------------------------

	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
		break;
	case WM_SIZE:
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			break;
		case VK_RIGHT:
			g_client.getPlayerData()->pos.x += value;
			break;
		case VK_LEFT:
			g_client.getPlayerData()->pos.x -= value;
			break;
		case VK_DOWN:
			g_client.getPlayerData()->pos.y += value;
			break;
		case VK_UP:
			g_client.getPlayerData()->pos.y -= value;
			break;

		default:
#ifdef _DEBUG
			// 디버깅 모드에서 예외되는 키를 입력할 경우, 서버와 기본 연결 테스트 통신을 하게 된다.
			//g_client.sendPacket_TEST();
#endif // _DEBUG
			return 0;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);

		// 1. 작업해야 하는 것들, 맨 처음 클라이언트 통신 후 주고받아야 하는 데이터들 주고 받기
		// 2. 다른 플레이어 데이터 벡터에 저장하기
		// 3. 시야 범위 내에 있는 플레이어 노출시키기
		{
			player_data temp;
			temp = *(g_client.getPlayerData());
			g_client.sendPacket(sizeof(player_data), CHANGED_POSITION, reinterpret_cast<BYTE*>(&temp));
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		Ellipse(hdc, g_client.getPlayerData()->pos.x - view_range, g_client.getPlayerData()->pos.y - view_range, g_client.getPlayerData()->pos.x + view_range, g_client.getPlayerData()->pos.y + view_range);
		Ellipse(hdc, g_client.getPlayerData()->pos.x - value, g_client.getPlayerData()->pos.y - value, g_client.getPlayerData()->pos.x + value, g_client.getPlayerData()->pos.y + value);

		for (auto players : *(g_client.getOtherPlayers())) {
			Ellipse(hdc, players.second.pos.x - value, players.second.pos.y - value, players.second.pos.x + value, players.second.pos.y + value);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_SOCKET:
		g_client.ProcessWinMessage(hWnd, uMsg, wParam, lParam);
		return 0;
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}