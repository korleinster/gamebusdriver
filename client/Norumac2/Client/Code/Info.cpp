#include "stdafx.h"
#include "Info.h"




CInfo::CInfo(const D3DXVECTOR3 & vLook)
{
	for (int i = 0; i < ANGLE_END; ++i)
		m_fAngle[i] = 0.f;

	m_vScale = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_vDir = vLook;
	D3DXMatrixIdentity(&m_matWorld);
}

CInfo::~CInfo()
{
}

int CInfo::Update(void)
{
	D3DXMATRIX	matScale, matRotX, matRotY, matRotZ, matTrans;

	//스자이공
	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationX(&matRotX, m_fAngle[ANGLE_X]);
	D3DXMatrixRotationY(&matRotY, m_fAngle[ANGLE_Y]);
	D3DXMatrixRotationZ(&matRotZ, m_fAngle[ANGLE_Z]);
	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
	return 0;
}

DWORD CInfo::Release(void)
{
	return 0;
}

CInfo * CInfo::Create(const D3DXVECTOR3 & vLook)
{

	return new CInfo(vLook);
}
