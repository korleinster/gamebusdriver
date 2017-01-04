#include "stdafx.h"
#include "Input.h"



IMPLEMENT_SINGLETON(CInput)

CInput::CInput(void)
	: m_pInput(NULL),
	m_pKeyBoard(NULL),
	m_pMouse(NULL)
{
	ZeroMemory(m_byKetState, sizeof(BYTE) * 256);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

CInput::~CInput(void)
{
	Release();
}

BYTE CInput::GetDIKeyState(BYTE KeyFlag)
{
	return m_byKetState[KeyFlag];
}

BYTE CInput::GetDIMouseState(MOUSECLICK KeyFlag)
{
	return m_MouseState.rgbButtons[KeyFlag];
}

long CInput::GetDIMouseMove(MOUSEMOVE KeyFlag)
{
	return *(((long*)&m_MouseState) + KeyFlag);
}

void CInput::SetInputState(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKetState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

HRESULT CInput::InitInputDevice(HINSTANCE hInst, HWND hWnd)
{
	HRESULT	hr = NULL;


	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL);
	FAILED_CHECK_MSG(hr, L"입력장치 생성 실패");

	hr = InitKeyBoard(hWnd);
	FAILED_CHECK_MSG(hr, L"키보드 생성 실패");

	hr = InitMouse(hWnd);
	FAILED_CHECK_MSG(hr, L"마우스 생성 실패");
	return S_OK;
}

HRESULT CInput::InitKeyBoard(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL);
	FAILED_CHECK(hr);

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	FAILED_CHECK(hr);

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	FAILED_CHECK(hr);

	m_pKeyBoard->Acquire();

	return S_OK;
}

HRESULT CInput::InitMouse(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	FAILED_CHECK(hr);

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	FAILED_CHECK(hr);

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	FAILED_CHECK(hr);

	m_pMouse->Acquire();
	return S_OK;
}

void CInput::Release(void)
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInput);
}

