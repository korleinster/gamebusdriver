#pragma once

//#include "Include.h"
//
//class CFont;
//class CFontMgr
//{
//public:
//	DECLARE_SINGLETON(CFontMgr)
//	explicit CFontMgr(void);
//	virtual ~CFontMgr(void);
//public:
//	HRESULT AddFont(LPDIRECT3DDEVICE9 pDevice, const TCHAR* pFontKey
//		, const UINT& iHeight, const UINT& iWidth, const UINT& iWeight);
//	void Render(const TCHAR* pFontKey, const D3DXVECTOR3& vPos, const TCHAR* pString
//		, D3DXCOLOR Color);
//
//private:
//	typedef map<const TCHAR*, CFont*>		MAPFONT;
//	MAPFONT			m_mapFont;
//public:
//	void Release();
//};