#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>

#include"resource.h"
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

#include<iostream>
#include<unordered_map>
using namespace std;

#include<chrono>
using namespace chrono;

// 서버와 연동되는 protocol.h - 수정시 서버에도 바로 적용된다.
//#include"../../../server/Server/Server/protocol.h"
#include"../../../server/2017 server/serverBoostModel/serverBoostModel/protocol.h"
#define WM_SOCKET (WM_USER + 1)

// client 통신 class
#include<fstream>
#include"clientClass.h"

static AsynchronousClientClass g_client;

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
