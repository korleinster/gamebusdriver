#pragma once
#include "stdafx.h"

// call back function 접근용 포인터
win_main_class* win_main_class::me = nullptr;

win_main_class::win_main_class(const _In_ HINSTANCE hInstance, const _In_ int nCmdShow)
{
	me = this;
	m_hInst = hInstance;

	m_WndClass.cbClsExtra = 0;
	m_WndClass.cbWndExtra = 0;
	m_WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 흰색 배경
	m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);				// 기본 커서
	m_WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// 좌상단 아이콘
	m_WndClass.hInstance = hInstance;
	m_WndClass.lpfnWndProc = m_WndProc;
	m_WndClass.lpszClassName = L"2017 Chat Client";
	m_WndClass.lpszMenuName = NULL;
	m_WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&m_WndClass);

	// 창 크기 고정용
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	static RECT window_size = { 0 , 0, 1024, 768 };
	AdjustWindowRect(&window_size, dwStyle, FALSE);

	m_hWnd = CreateWindow(m_WndClass.lpszClassName, m_WndClass.lpszClassName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(m_hWnd, nCmdShow);
	
	m_network.init(m_hWnd, hInstance, &m_player);

	while (GetMessage(&m_Message, NULL, 0, 0))
	{
		TranslateMessage(&m_Message);
		DispatchMessage(&m_Message);
	}
}

win_main_class::~win_main_class()
{

}

LRESULT CALLBACK win_main_class::m_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	switch (message)
	{
	case WM_PAINT: {

		//int ratio = { 30 };	// player 전체 기준 반지름 & 한 블럭당 크기
		//int ratio_other = { 5 }; // 다른 플레이어 반지름 감소 비율
		//me->m_hdc = BeginPaint(hWnd, &me->m_ps);

		//// player 위치 찾아 그리기
		//Ellipse(me->m_hdc,
		//	me->m_player.get_pos()->x * ratio,
		//	me->m_player.get_pos()->y * ratio,
		//	me->m_player.get_pos()->x * ratio + ratio,
		//	me->m_player.get_pos()->y * ratio + ratio);

		//// map 그리기
		//int draw_for_x = 0;
		//int draw_for_y = 0;
		//for (int i = 0; i < MAX_MAP_SIZE; ++i)
		//{
		//	draw_for_y += ratio;
		//	for (int j = 0; j < MAX_MAP_SIZE; ++j) {
		//		draw_for_x += ratio;
		//		MoveToEx(me->m_hdc, draw_for_x - ratio, draw_for_y, NULL);
		//		LineTo(me->m_hdc, draw_for_x, draw_for_y);
		//		MoveToEx(me->m_hdc, draw_for_x, draw_for_y, NULL);
		//		LineTo(me->m_hdc, draw_for_x, draw_for_y - ratio);
		//	}
		//	draw_for_x = 0;
		//}

		//// 딴 플레이어 그리기
		//for (auto other_players : *me->m_player.get_other_players()) {
		//	Ellipse(me->m_hdc,
		//		other_players.second.x * ratio + ratio_other,
		//		other_players.second.y * ratio + ratio_other,
		//		other_players.second.x * ratio + ratio - ratio_other,
		//		other_players.second.y * ratio + ratio - ratio_other);
		//}

		//EndPaint(hWnd, &me->m_ps);
		break;
	}
	case WM_SOCKET: {
		me->m_network.process_win_msg(lParam);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_KEYDOWN: {
		/*cs_packet_move packet;
		packet.input_key = me->m_player.key_input(wParam);
		me->m_network.send_packet(packet.size, packet.type, &packet);*/
		break;
	}
	case WM_CREATE: {

		break;
	}
	case WM_DESTROY: {

		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}