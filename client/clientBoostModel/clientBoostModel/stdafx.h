#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>


#include<iostream>
using namespace std;

// boost asio 서버와 연동되는 protocol.h - 수정시 서버에도 바로 적용된다.
#include"../../../server\serverBoostModel/serverBoostModel/protocol.h"
#define WM_SOCKET (WM_USER + 1)

// client 통신 관련
#include<fstream>

// 디버깅 전용 콘솔창
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif