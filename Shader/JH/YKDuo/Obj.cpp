#include "stdafx.h"
#include "Obj.h"
#include "Camera.h"
#include "Info.h"


CObj::CObj()
	:m_pInfo(NULL), m_bZSort(false)
{
}


CObj::~CObj()
{
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

