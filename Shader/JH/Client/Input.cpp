#include "stdafx.h"
#include "Input.h"
#include "Include.h"

IMPLEMENT_SINGLETON(CInput)

CInput::CInput(void)
	:m_pInput(NULL)
	, m_pKeyBoard(NULL)
	, m_pMouse(NULL)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}
CInput::~CInput(void)
{
	Release();
}

BYTE CInput::GetDIKeyState(BYTE KeyID)
{
	return m_byKeyState[KeyID];
}

BYTE CInput::GetDIMouseState(MOUSECLICK eKeyID)
{
	return m_MouseState.rgbButtons[eKeyID];
}
long CInput::GetDIMouseMove(MOUSEMOVE eKeyID)
{
	return *(((long*)&m_MouseState) + eKeyID);
}

void CInput::SetInputState(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

HRESULT CInput::Initialize(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = NULL;
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL);
	FAILED_CHECK(hr);

	hr = InitKeyBoard(hWnd);
	FAILED_CHECK(hr);

	hr = InitMouse(hWnd);
	FAILED_CHECK(hr);
	return S_OK;
}
HRESULT CInput::InitKeyBoard(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL);
	FAILED_CHECK(hr);

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	m_pKeyBoard->Acquire();
	return S_OK;
}

HRESULT CInput::InitMouse(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	FAILED_CHECK(hr);

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	m_pMouse->Acquire();
	return S_OK;
}

void CInput::Release()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInput);
}
