#include"stdafx.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 그리기 위한 변수 - WM_PAINT
int view_range{ 150 };
int value{ 1 * 5 };	// 원 충돌 범위 크기
double dir_value{ 0.1 * 20 }; // 이동 거리 및 방향
double att_value{ 0.1 * 20 }; // 공격 리치
int hp_value_x{ 25 };
// ------------------------------

// 키 조작 관련 함수 - WM_KEYDOWN
void move_up();
void move_down();
void move_left();
void move_right();

bool is_key_s_pushed{ false };

// 시간 체크 관련 클래스 - 공격키 s 입력
class TimeCheck
{
public:
	TimeCheck() { key_s = system_clock::now(); };
	~TimeCheck() {};
	
	void	start_key_s()	{ key_s = system_clock::now(); }
	UINT	end_key_s()		{ return duration_cast<milliseconds>(system_clock::now() - key_s).count(); }
private:
	system_clock::time_point key_s;
};

TimeCheck timecheck;

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
	g_client.Init(hWnd, hInstance);

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

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	switch (iMessage)
	{
	case WM_INITDIALOG: {

		SetDlgItemText(hDlg, IDC_IPADDR, L"127.0.0.1");
		SetDlgItemText(hDlg, IDC_ID, L"guest");
		SetDlgItemText(hDlg, IDC_PW, L"guest");

		return TRUE;
	}
	case WM_COMMAND: {

		switch (LOWORD(wParam))
		{
		case IDOK:
			wchar_t ip[32];

			GetDlgItemText(hDlg, IDC_IPADDR, ip, 32);
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



		// 일반 문자 key 를 입력 받았을 때
	case WM_CHAR:
	{
		switch (wParam)
		{
		case 's':	// attack key
			is_key_s_pushed = false;
			if (600 <= timecheck.end_key_s()) {
				g_client.sendPacket(0, KEYINPUT_ATTACK, nullptr);
				timecheck.start_key_s();
				is_key_s_pushed = true;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case 'i': {	// inventory check key
#ifdef _DEBUG
			/*cout << "\n\nhead : ";
			if (NONE == g_client.getPlayerData()->inven.head) {	cout << "NONE\n"; }
			else { cout << "BASIC_HEAD\n"; }

			cout << "body : ";
			if (NONE == g_client.getPlayerData()->inven.body) { cout << "NONE\n"; }
			else { cout << "BASIC_BODY\n"; }

			cout << "arm : ";
			if (NONE == g_client.getPlayerData()->inven.arm) { cout << "NONE\n"; }
			else { cout << "BASIC_ARM\n"; }

			cout << "weapon : ";
			if (NONE == g_client.getPlayerData()->inven.weapon) { cout << "NONE\n"; }
			else { cout << "BASIC_WEAPON\n"; }*/
#endif // _DEBUG
		}
			break;
		default:
			break;
		}
	}
		break;


		// key 를 입력 받았을 때
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			break;
		case VK_RIGHT:
			move_right();
			break;
		case VK_LEFT:
			move_left();
			break;
		case VK_DOWN:
			move_down();
			break;
		case VK_UP:
			move_up();
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);

		// 1. 작업해야 하는 것들, 맨 처음 클라이언트 통신 후 주고받아야 하는 데이터들 주고 받기
		// 2. 다른 플레이어 데이터 벡터에 저장하기
		// 3. 시야 범위 내에 있는 플레이어 노출시키기
		{
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&g_client.getPlayerData()->pos));
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&g_client.getPlayerData()->dir));
		}
		break;


	// 창에 그릴 때
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);

		char *dir;
		float x = g_client.getPlayerData()->pos.x, y = g_client.getPlayerData()->pos.y;

		/// this player view circle
		Ellipse(hdc, x - view_range, y - view_range, x + view_range, y + view_range);
		
		/// other player ---------------------------------------------------------------------------------
		for (auto players : *(g_client.getOtherPlayers())) {
			x = players.second.pos.x;
			y = players.second.pos.y;
			if (players.second.id < 50) { Rectangle(hdc, x - value, y - value, x + value, y + value); }	// ai
			else { Ellipse(hdc, x - value, y - value, x + value, y + value); }	// player

			/// player direction
			dir = &players.second.dir;
			if ((*dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT)) { x += (value + dir_value); }
			if ((*dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT)) { x -= (value + dir_value); }
			if ((*dir & KEYINPUT_UP) == (KEYINPUT_UP)) { y -= (value + dir_value); }
			if ((*dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN)) { y += (value + dir_value); }
			Ellipse(hdc, x - dir_value, y - dir_value, x + dir_value, y + dir_value);

			/// HP bar - other_player
			if (players.second.state.maxhp == players.second.state.hp) { continue; }
			x = players.second.pos.x;
			y = players.second.pos.y;
			int other_player_hp = players.second.state.hp;
			RECT other_player_hp_bar;
			HBRUSH other_player_hp_color = CreateSolidBrush(RGB(0, 100, 255));
			other_player_hp_bar.left = x - hp_value_x;
			if (1 < other_player_hp) { other_player_hp_bar.right = other_player_hp_bar.left + (other_player_hp / (players.second.state.maxhp / (2 * hp_value_x)));/*other_player_hp_bar.left + (other_player_hp / 5);*/ }
			other_player_hp_bar.top = y - 21;
			other_player_hp_bar.bottom = y - 16;
			if (other_player_hp < (players.second.state.maxhp * 0.9)) { other_player_hp_color = CreateSolidBrush(RGB(0, 255, 0)); }
			if (other_player_hp <= (players.second.state.maxhp * 0.5)) { other_player_hp_color = CreateSolidBrush(RGB(255, 0, 0)); }
			FillRect(hdc, &other_player_hp_bar, other_player_hp_color);
			other_player_hp_bar.right = x + hp_value_x;
			FrameRect(hdc, &other_player_hp_bar, CreateSolidBrush(RGB(0, 0, 0)));
		}

		/// this player ---------------------------------------------------------------------------------
		dir = &(g_client.getPlayerData()->dir);
		x = g_client.getPlayerData()->pos.x, y = g_client.getPlayerData()->pos.y;
				
		/// player circle
		Ellipse(hdc, x - value, y - value, x + value, y + value);

		/// player direction
		if ((*dir & KEYINPUT_RIGHT)	== (KEYINPUT_RIGHT)){ x += (value + dir_value); }
		if ((*dir & KEYINPUT_LEFT)	== (KEYINPUT_LEFT))	{ x -= (value + dir_value); }
		if ((*dir & KEYINPUT_UP)	== (KEYINPUT_UP))	{ y -= (value + dir_value); }
		if ((*dir & KEYINPUT_DOWN)	== (KEYINPUT_DOWN))	{ y += (value + dir_value); }
		
		/// 공격이 눌린 모션 진행 중이면, 공격 X 를.. 아니면 보는 방향 원을 그리기
		if ((100 >= timecheck.end_key_s()) && is_key_s_pushed) {
			MoveToEx(hdc, x - att_value, y - att_value, nullptr);
			LineTo(hdc, x + att_value, y + att_value);
			MoveToEx(hdc, x - att_value, y + att_value, nullptr);
			LineTo(hdc, x + att_value, y - att_value);
		}
		else {
			Ellipse(hdc, x - dir_value, y - dir_value, x + dir_value, y + dir_value);
			is_key_s_pushed = false;
		}
		
		/// HP bar - player
		x = g_client.getPlayerData()->pos.x, y = g_client.getPlayerData()->pos.y;
		int hp = g_client.getPlayerData()->state.hp;
		RECT hp_bar;
		HBRUSH hp_color = CreateSolidBrush(RGB(0, 100, 255));
		hp_bar.left = x - hp_value_x;
		if (1 < hp){ hp_bar.right = hp_bar.left + (hp / (g_client.getPlayerData()->state.maxhp / (2 * hp_value_x))); }
		hp_bar.top = y - 21;
		hp_bar.bottom = y - 16;
		if (hp < 90) { hp_color = CreateSolidBrush(RGB(0, 255, 0)); }
		if (hp <= 50) { hp_color = CreateSolidBrush(RGB(255, 0, 0)); }
		FillRect(hdc, &hp_bar, hp_color);
		hp_bar.right = x + hp_value_x;
		FrameRect(hdc, &hp_bar, CreateSolidBrush(RGB(0, 0, 0)));

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SOCKET:
		g_client.ProcessWinMessage(hWnd, uMsg, wParam, lParam);
		return 0;
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



void move_up() {
	g_client.getPlayerData()->pos.y -= 10;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir | KEYINPUT_UP;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_DOWN);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_RIGHT);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_LEFT);
}
void move_down() {
	g_client.getPlayerData()->pos.y += 10;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir | KEYINPUT_DOWN;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_UP);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_RIGHT);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_LEFT);
}
void move_left() {
	g_client.getPlayerData()->pos.x -= 10;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir | KEYINPUT_LEFT;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_UP);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_DOWN);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_RIGHT);
}
void move_right() {
	g_client.getPlayerData()->pos.x += 10;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir | KEYINPUT_RIGHT;
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_UP);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_DOWN);
	g_client.getPlayerData()->dir = g_client.getPlayerData()->dir & (~KEYINPUT_LEFT);
}