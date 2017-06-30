#pragma once

class win_main_class
{
	static win_main_class* me;

	// window class
	HWND m_hWnd;
	MSG m_Message;
	WNDCLASS m_WndClass;
	HINSTANCE m_hInst;

	// 그리기 위한 변수 - WM_PAINT
	HDC m_hdc;
	PAINTSTRUCT m_ps;
	
	// call back function
	static LRESULT CALLBACK m_WndProc(HWND, UINT, WPARAM, LPARAM);

	// network
	network_class m_network;
	player_class m_player;

public:
	win_main_class(const _In_ HINSTANCE hInstance, const _In_ int nCmdShow);
	~win_main_class();

	MSG* getMsg() { return &m_Message; }
};