#include "stdafx.h"
#include "DynamicCamera.h"
#include "include.h"
#include "Input.h"


CDynamicCamera::CDynamicCamera()
	: m_fCamSpeed(0.f)
	, m_bClick(false)
	, m_bMouseFix(true)
{

}

CDynamicCamera::~CDynamicCamera(void)
{

}

HRESULT CDynamicCamera::InitCamera(const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt)
{
	m_fCamSpeed = 10.f;
	m_vEye = *pEye;
	m_vAt = *pAt;
	MakeView();

	m_fFovY = D3DXToRadian(45.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	MakeProjection();
	return S_OK;
}

int CDynamicCamera::Update()
{
	KeyCheck();
	return 0;
}

void CDynamicCamera::KeyCheck()
{
	if (CInput::GetInstance()->GetDIKeyState(DIK_W) && 0x80)
	{
		cout << "w" << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_S) && 0x80)
	{
		cout << "s" << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_A) && 0x80)
	{
		cout << "a" << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_D) && 0x80)
	{
		cout << "d" << endl;
	}
}

void CDynamicCamera::MouseMove()
{
	if (CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
	{
		cout << "horizon" << endl;
	}

	if (CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y))
	{
		cout << "vertical" << endl;
	}
}