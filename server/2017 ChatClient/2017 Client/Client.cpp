#include "stdafx.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

	_wsetlocale(LC_ALL, L"korean");
	win_main_class win_main_class(hInstance, nCmdShow);

	return (int)win_main_class.getMsg()->wParam;
}