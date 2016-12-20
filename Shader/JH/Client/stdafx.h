// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

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
#include <D3DX10Math.h>
#include <d3dcompiler.h>
#include <xnamath.h>

//#include "d3dx11effect.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>



#include <fbxsdk.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <ctime>
#include <iostream>
#include <process.h>



#include "Include.h"

using namespace std;

#pragma warning(disable : 4005)
#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif