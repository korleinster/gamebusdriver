#pragma once

#include "Include.h"

class CFont;
class CFontMgr
{
public:
	DECLARE_SINGLETON(CFontMgr)
	explicit CFontMgr(void);
	virtual ~CFontMgr(void);
public:
	HRESULT AddFont(const LPCWSTR& strFontPath, const wstring& strFontKey, const wstring& strFontName);
	CFont* CloneFont(const wstring strFontKey);
	CFont* FindFont(const wstring strResourceKey);

private:
	typedef map<wstring, CFont*>		MAPFONT;
	MAPFONT			m_mapFont;
public:
	void Release();
};