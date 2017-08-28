#include "stdafx.h"
#include "FontMgr.h"
#include "Font.h"
#include "Include.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr(void)
{

}

CFontMgr::~CFontMgr(void)
{

}

HRESULT CFontMgr::AddFont(const LPCWSTR& strFontPath, const wstring& strFontKey, const wstring& strFontName)
{
	CFont*		pFont = FindFont(strFontKey);

	if (pFont != NULL)
		return E_FAIL;

	HRESULT hr = AddFontResourceEx(strFontPath, FR_PRIVATE, NULL);
	if (hr == E_FAIL)
		return E_FAIL;

	//pFont = CFont::Create(strFontName);
	NULL_CHECK_RETURN(pFont, E_FAIL);

	m_mapFont.insert(MAPFONT::value_type(strFontKey, pFont));
	return S_OK;
}


CFont * CFontMgr::FindFont(const wstring strResourceKey)
{
	MAPFONT::iterator iter = m_mapFont.find(strResourceKey);

	if (iter == m_mapFont.end())
	{
		return NULL;
	}

	return iter->second;
}

CFont* CFontMgr::CloneFont(const wstring strFontKey)
{
	CFont* pFont = FindFont(strFontKey);

	if (pFont == NULL)
		return NULL;

	return pFont->CloneFont();
}

void CFontMgr::Release(void)
{
	MAPFONT::iterator	iter = m_mapFont.begin();
	MAPFONT::iterator	iter_end = m_mapFont.end();

	for (; iter != iter_end; ++iter)
	{
		Safe_Delete(iter->second);
	}
	m_mapFont.clear();
}

