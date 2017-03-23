// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

// 서버와 관련된 헤더 파일들
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>


#include "targetver.h"
#include "resource.h"
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <d3d9.h>
#include <d3dx9.h>

#include <d3d11.h>
#include <d3dx11.h>
//#include "..\..\..\DX9\D3DX10Math.h"
//#include "..\..\..\DX9\d3dx9math.h"
#include <d3dcompiler.h>
#include <xnamath.h>

#include <fbxsdk.h>

// #include "d3dx11effect.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <ctime>
#include <iostream>
#include <process.h>
#include <assert.h>
#include <unordered_map>

#include "Include.h"

using namespace std;

// 서버와 관련된 프로토콜
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"

// client 통신 관련 class
#include<fstream>
#include"../Server_Code/ClientClass.h"

extern AsynchronousClientClass g_client;

#pragma warning(disable : 4005)
#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif