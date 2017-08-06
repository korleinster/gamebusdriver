#pragma once

#include "Enum.h"
#include "Value.h"
#include "Macro.h"
#include "Struct.h"
#include "../../../../server/2017 server/serverBoostModel/serverBoostModel/protocol.h"


//전역변수
extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern DWORD		g_dwLightIndex;
extern D3DXVECTOR3	g_vLightDir;
extern bool			g_bLogin;
extern bool			g_bChatMode;
extern bool			g_bChatEnd;
extern float		g_fChatCool;


class CStringCompare
{
public:
	explicit CStringCompare(const TCHAR* pKey)
		: m_pString(pKey) {}
	~CStringCompare() {}
public:
	template <typename T>
	bool operator () (T Data)
	{
		return !lstrcmp(Data.first, m_pString);
	}
private:
	const TCHAR*	m_pString;
};


