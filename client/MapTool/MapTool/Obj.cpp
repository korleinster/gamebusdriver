#include "stdafx.h"
#include "Obj.h"
#include "Camera.h"
#include "Info.h"
#include "Device.h"


CObj::CObj()
	:m_pInfo(NULL), m_bZSort(false), m_pGrapicDevice(CDevice::GetInstance()), m_eReleaseType(Release_End)
{
	ZeroMemory(m_tcKey, sizeof(TCHAR) * 256);
}


CObj::~CObj()
{
	Release();
}

HRESULT CObj::Initialize(void)
{
	return S_OK;
}

int CObj::Update(void)
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second->Update();
	}


	if (m_bZSort == true)
	{
		if (m_pInfo != NULL)
			Compute_ViewZ(&m_pInfo->m_vPos);
	}

	return 0;
}

CObj * CObj::Create(ReleaseType _eType)
{
	CObj* pObj = new CObj;

	if (FAILED(pObj->Initialize()))
	{
		::Safe_Delete(pObj);
	}

	pObj->m_eReleaseType = _eType;
	return pObj;
}

void CObj::Release(void)
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();

	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(iter->second);
	}
	m_mapComponent.clear();
}

void CObj::SetPos(D3DXVECTOR3 vPos)
{
	if (m_pInfo != NULL)
		m_pInfo->m_vPos = vPos;
}

void CObj::SetScale(D3DXVECTOR3 vScale)
{
	if (m_pInfo != NULL)
		m_pInfo->m_vScale = vScale;
}

void CObj::SetAngle(D3DXVECTOR3 vAngle)
{
	if (m_pInfo != NULL)
	{
		m_pInfo->m_fAngle[ANGLE_X] = vAngle.x;
		m_pInfo->m_fAngle[ANGLE_Y] = vAngle.x;
		m_pInfo->m_fAngle[ANGLE_Z] = vAngle.x;
	}
}

void CObj::Compute_ViewZ(const D3DXVECTOR3 * pPos)
{
	D3DXMATRIX		matView;
	D3DXMatrixIdentity(&matView);
	//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	matView = CCamera::GetInstance()->m_matView;

	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVECTOR3		vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(D3DXVECTOR3));

	float		fDistance = D3DXVec3Length(&(vCamPos - *pPos));

	m_fViewZ = fDistance;
}

const float & CObj::GetViewZ(void) const
{
	return m_fViewZ;
}

const CComponent* CObj::GetComponent(const wstring& _wstrComponentKey)
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.find(_wstrComponentKey);

	if (iter == m_mapComponent.end())
		return NULL;

	return iter->second;
}
