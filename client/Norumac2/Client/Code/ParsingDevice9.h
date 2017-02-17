#pragma once

class CParsingDevice9
{
public:
	CParsingDevice9();
	~CParsingDevice9();

	DECLARE_SINGLETON(CParsingDevice9)

public:
	enum WINMODE { MODE_FULL, MODE_WIN };

public:
	HRESULT InitGraphicDev(WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);

public:
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pDevice; // 장치를 대표하는 객체다.

public:
	void SetParameters(D3DPRESENT_PARAMETERS& d3dpp,WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);
	void Release(void);
};

