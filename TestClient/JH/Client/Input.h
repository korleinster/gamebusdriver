#pragma once
#ifndef Input_h__
#define Input_h__

#include "Include.h"

class CInput
{
public :
	DECLARE_SINGLETON(CInput)

public:
	enum MOUSECLICK { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON };
	enum MOUSEMOVE { DIM_X, DIM_Y, DIM_Z };

public:
	CInput(void);
	virtual ~CInput(void);

public:
	BYTE GetDIKeyState(BYTE KeyID);
	BYTE GetDIMouseState(MOUSECLICK eKeyID);
	long GetDIMouseMove(MOUSEMOVE eKeyID);
public:
	void SetInputState(void);

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void Release(void);

private:
	HRESULT InitKeyBoard(HWND hWnd);
	HRESULT InitMouse(HWND hWnd);

private:
	LPDIRECTINPUT8				m_pInput;
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

private:
	BYTE				m_byKeyState[256];
	DIMOUSESTATE		m_MouseState;

};
#endif // Input_h__