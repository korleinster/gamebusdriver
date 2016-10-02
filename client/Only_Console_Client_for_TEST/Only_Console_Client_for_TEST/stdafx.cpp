// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Only_Console_Client_for_TEST.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SOCKET:
		client.ProcessWinMessage(hwnd, uMsg, wParam, lParam);
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		cout << "CALLBACK WndProc ERROR\n";
		exit(-1);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}