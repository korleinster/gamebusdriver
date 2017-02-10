//#pragma once
//
//#ifndef Font_h__
//#define Font_h__
//
//#include "Resources.h"
//#include "FW1FontWrapper.h"
//
//class CFont : public CResources
//{
//
//private:
//	IFW1FontWrapper*		m_pFW1FontWarpper;
//	IFW1Factory*			m_pFW1FontFactory;
//	//IDWriteFactory*			m_pWriteFactory;
//
//	ID3D11DeviceContext*	m_pDeivceContext;
//
//
//
//
//public:
//	FontType		m_eType;
//
//	wstring			m_wstrText;
//	float			m_fSize;
//	UINT32			m_nColor;
//	UINT			m_nFlag;
//
//	D3DXVECTOR2		m_vPos;
//
//	float			m_fOutlineSize;
//	UINT32			m_nOutlineColor;
//
//
//public:
//	static CFont*		Create(const wstring& _wstPath);
//	virtual CResources* CloneResource(void);
//
//private:
//	HRESULT			Load_Font(const wstring& _wstPath);
//
//public:
//	HRESULT			Initialize(const wstring& _wstPath);
//	void			Render();
//	DWORD			Release();
//
//
//protected:
//	 CFont();
//public:
//	virtual ~CFont();
//};
//
//#endif // Font_h__
