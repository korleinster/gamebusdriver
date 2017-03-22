#pragma once


class CInput
{
private:
	CInput(void);
	~CInput(void);

public:
	DECLARE_SINGLETON(CInput)

public:
	enum MOUSECLICK { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON };
	enum MOUSEMOVE { DIM_X, DIM_Y, DIM_Z };


public:
	BYTE GetDIKeyState(BYTE KeyFlag);
	BYTE GetDIMouseState(MOUSECLICK KeyFlag);
	long GetDIMouseMove(MOUSEMOVE KeyFlag);

public:
	void SetInputState(void);
	HRESULT InitInputDevice(HINSTANCE hInst, HWND hWnd);

private:
	HRESULT	InitKeyBoard(HWND hWnd);
	HRESULT	InitMouse(HWND hWnd);
	void	Release(void);

private:
	LPDIRECTINPUT8			m_pInput;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:
	BYTE					m_byKetState[256];
	BYTE					m_bKey;
	DIMOUSESTATE			m_MouseState;

};

