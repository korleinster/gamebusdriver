#include "stdafx.h"
#include "Camera.h"
#include "TimeMgr.h"
#include "Info.h"
#include "Input.h"
#include "SceneMgr.h"
#include "Interface.h"
#include "MainFrm.h"
#include "MyForm.h"
#include "Device.h"

IMPLEMENT_SINGLETON(CCamera)

CCamera::CCamera()
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

}


CCamera::~CCamera()
{
	Release();
}

HRESULT CCamera::Initialize(void)
{
	//m_vEye = D3DXVECTOR3(0.0f, 20.f, -20.0f);
	m_vEye = D3DXVECTOR3(-70.0f, 210.f, -200.0f);
	m_vAt = D3DXVECTOR3(10.f, 10.f, 10.f);
	m_vUp = D3DXVECTOR3(0.f, 1.f, 0.f);

	m_vDirZ = D3DXVECTOR3(0.f, 0.f, -1.f);

	m_bMouseFix = true;

	m_pInfo = CInfo::Create(D3DXVECTOR3(0.f, 0.f, 1.f));

	m_fFovY = D3DXToRadian(45.f);
	m_fAspect = float(WINCX) / static_cast<float>(WINCY);
	m_fNear = 1.f;
	m_fFar = 1000.f;
	MakeView();
	MakeProjection();

	m_fCameraDistance = 10.f;
	m_fCameraSpeed = 70.f;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;

	cbd.ByteWidth = sizeof(SelectBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_cbSelectBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"¿Ö ¾ÈµÅÁö?", L"Selcet Constant Buffer Error", MB_OK);
		return hr;
	}


	D3DXVECTOR4 vSelect(SELECT_OFF, SELECT_OFF, SELECT_OFF, SELECT_OFF);
	CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(CCamera::GetInstance()->m_cbSelectBuffer, 0, NULL, &vSelect, 0, 0);


	return S_OK;
}

int CCamera::Update(void)
{

	m_pInfo->Update();

	CInterface* pInterFace = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_InterFace;
	pInterFace->CamUpdate(m_vEye, m_vAt,m_bMouseFix,m_fCameraSpeed);


	KeyState();
	MouseMove();
	FixMouse();
	MakeView();
	MakeProjection();
	return 0;
}

void CCamera::MakeView(void)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
}

void CCamera::MakeProjection(void)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
}

void CCamera::Release(void)
{
}

void CCamera::KeyState(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();


	if (CInput::GetInstance()->GetDIKeyState(DIK_O) & 0x80)
	{
		if (m_bMouseFix == true)
			m_bMouseFix = false;


	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_P) & 0x80)
	{
		if (m_bMouseFix == false)
			m_bMouseFix = true;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_A) & 0x80)
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;

		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye -= vRight * m_fCameraSpeed * fTime;
		m_vAt -= vRight * m_fCameraSpeed * fTime;
		//m_vAt.x -= m_fCameraSpeed * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_D) & 0x80)
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;

		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye += vRight * m_fCameraSpeed * fTime;
		m_vAt += vRight * m_fCameraSpeed * fTime;

		//m_vAt.x += m_fCameraSpeed * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_W) & 0x80)
	{
		D3DXVECTOR3		vLook;
		vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye += vLook * m_fCameraSpeed * fTime;
		m_vAt += vLook * m_fCameraSpeed * fTime;

		//m_pInfo->m_vDir.y = 0.f;
		//m_vAt += m_pInfo->m_vDir * fTime * m_fCameraSpeed * -1.f;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_S) & 0x80)
	{
		D3DXVECTOR3		vLook;
		vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye -= vLook * m_fCameraSpeed * fTime;
		m_vAt -= vLook * m_fCameraSpeed * fTime;

		//m_pInfo->m_vDir.y = 0.f;
		//m_vAt -= m_pInfo->m_vDir * fTime * m_fCameraSpeed * -1.f;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_NUMPADPLUS) & 0x80)
	{
		++m_fCameraSpeed;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_NUMPADMINUS) & 0x80)
	{
		--m_fCameraSpeed;
	}

}

void CCamera::FixMouse(void)
{
	POINT ptMouse;
	ZeroMemory(&ptMouse, sizeof(POINT));
	ptMouse.x;

	ptMouse.x = WINCX >> 1;
	ptMouse.y = WINCY >> 1;


	ClientToScreen(g_hWnd, &ptMouse);


	if (m_bMouseFix == true)
		SetCursorPos(ptMouse.x, ptMouse.y);

}

void CCamera::MouseMove(void)
{
	int iInput = 0;

	if (m_bMouseFix == false)
		return;


	float	fTime = CTimeMgr::GetInstance()->GetTime();
	int		iDistance = 0;

	//if (CInput::GetInstance()->GetDIKeyState(DIK_LCONTROL))
	//{
		if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
		{
			D3DXMATRIX		matAxis;
			D3DXMatrixRotationAxis(&matAxis, &D3DXVECTOR3(0.f, 1.f, 0.f), D3DXToRadian(iDistance / 10.f));

			D3DXVECTOR3		vDir;
			vDir = m_vAt - m_vEye;
			D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

			m_vAt = m_vEye + vDir;

		}

		if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y))
		{
			D3DXVECTOR3		vRight;
			D3DXMATRIX		matCamState;

			D3DXMatrixInverse(&matCamState, NULL, &m_matView);
			memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
			D3DXVec3Normalize(&vRight, &vRight);

			D3DXMATRIX		matAxis;
			D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(iDistance / 10.f));

			D3DXVECTOR3		vDir;
			vDir = m_vAt - m_vEye;
			D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

			m_vAt = m_vEye + vDir;
		}
	//}
	//else
	//{
	//	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Z))
	//	{
	//		iInput = 1;
	//		m_fCameraDistance -= fTime * 800.f * iDistance * 0.01f;


	//		if (m_fCameraDistance < 5.f)
	//			m_fCameraDistance = 5.f;
	//		// 
	//		// 		if(m_fDistance > 700.f)
	//		// 			m_fDistance = 700.f;

	//		D3DXVec3Normalize(&m_vDirZ, &m_vDirZ);
	//		m_vDirZ *= m_fCameraDistance;

	//		m_vEye = m_vAt + m_vDirZ;

	//	}

	//	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
	//	{
	//		iInput = 1;
	//		D3DXMATRIX		matAxis;
	//		D3DXMatrixRotationAxis(&matAxis, &D3DXVECTOR3(0.f, 1.f, 0.f), D3DXToRadian(iDistance / 10.f));

	//		//D3DXVECTOR3		vDir;
	//		m_vDirZ = m_vEye - m_vAt;

	//		D3DXVec3TransformNormal(&m_vDirZ, &m_vDirZ, &matAxis);

	//		D3DXVec3Normalize(&m_vDirZ, &m_vDirZ);
	//		m_pInfo->m_vDir.x = m_vDirZ.x;
	//		m_pInfo->m_vDir.z = m_vDirZ.z;;
	//		m_vDirZ *= m_fCameraDistance;

	//		m_vEye = m_vAt + m_vDirZ;

	//	}

	//	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y))
	//	{
	//		iInput = 1;
	//		D3DXVECTOR3		vRight;
	//		D3DXMATRIX		matCamState;

	//		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
	//		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
	//		D3DXVec3Normalize(&vRight, &vRight);

	//		D3DXMATRIX		matAxis;

	//		D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(iDistance / 10.f));



	//		m_vDirZ = m_vEye - m_vAt;
	//		D3DXVec3TransformNormal(&m_vDirZ, &m_vDirZ, &matAxis);
	//		D3DXVec3Normalize(&m_vDirZ, &m_vDirZ);

	//		if (m_vDirZ.y > 0.75f)
	//			m_vDirZ.y = 0.75f;

	//		if (m_vDirZ.y < -0.35f)
	//			m_vDirZ.y = -0.35f;

	//		m_pInfo->m_vDir.x = m_vDirZ.x;
	//		m_pInfo->m_vDir.z = m_vDirZ.z;
	//		m_vDirZ *= m_fCameraDistance;
	//		m_vEye = m_vAt + m_vDirZ;
	//	}

	//	/*if (iInput == 0)
	//	{
	//		D3DXVec3Normalize(&m_vDirZ, &m_vDirZ);
	//		m_pInfo->m_vDir.x = m_vDirZ.x;
	//		m_pInfo->m_vDir.z = m_vDirZ.z;
	//		m_vDirZ *= m_fCameraDistance;

	//		m_vEye = m_vAt + m_vDirZ;
	//	}*/

	//	D3DXVec3Normalize(&m_pInfo->m_vDir, &m_pInfo->m_vDir);
	//}



}