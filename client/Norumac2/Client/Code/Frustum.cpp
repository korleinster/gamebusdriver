#include "stdafx.h"
#include "Frustum.h"
#include "Include.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
	ZeroMemory(m_vPoint, sizeof(D3DXVECTOR3) * 8);
	ZeroMemory(m_Plane, sizeof(D3DXPLANE) * 6);

	Init_FrustumInProj();
}

CFrustum::~CFrustum()
{

}

bool CFrustum::Decision_In(const D3DXVECTOR3* pPosition, const float& fRadius)
{


	Transform_ToView();
	Transform_ToWorld();

	float fDistance = 0.f;

	// +x, -x
	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[0], &m_vPoint[3], &m_vPoint[7]);

	// +z, -z
	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	D3DXPlaneFromPoints(&m_Plane[3], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);

	for (int i = 0; i < 4; ++i)
	{
		fDistance = D3DXPlaneDotCoord(&m_Plane[i], pPosition);

		if (fDistance > fRadius)
			return false;
	}

	return true;


}

bool CFrustum::Decision_In(const D3DXVECTOR3* pPosition)
{

	Transform_ToView();
	Transform_ToWorld();

	// +x, -x	
	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[0], &m_vPoint[3], &m_vPoint[7]);

	// +z, -z
	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);

	float		fDistance = 0.f;

	for (int i = 0; i < 3; ++i)
	{
		fDistance = D3DXPlaneDotCoord(&m_Plane[i], pPosition);

		if (fDistance > 0.f)
			return false;
	}

	return true;

}

bool CFrustum::Decision_In_Object(const D3DXVECTOR3* pPosition, const float& fRadius)
{
	Transform_ToView();
	Transform_ToWorld();

	float fDistance = 0.f;

	// +x, -x
	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	fDistance = D3DXPlaneDotCoord(&m_Plane[0], pPosition);
	if (fDistance > fRadius)
		return false;

	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[3], &m_vPoint[7], &m_vPoint[4]);
	fDistance = D3DXPlaneDotCoord(&m_Plane[1], pPosition);
	if (fDistance > fRadius)
		return false;

	// +z, -z
	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	fDistance = D3DXPlaneDotCoord(&m_Plane[2], pPosition);
	if (fDistance > fRadius)
		return false;

	D3DXPlaneFromPoints(&m_Plane[3], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);
	fDistance = D3DXPlaneDotCoord(&m_Plane[3], pPosition);
	if (fDistance > fRadius)
		return false;

	return true;

}

void CFrustum::Init_FrustumInProj(void)
{
	m_vPoint[0] = D3DXVECTOR3(-1.f, 1.f, 0.f);
	m_vPoint[1] = D3DXVECTOR3(1.f, 1.f, 0.f);
	m_vPoint[2] = D3DXVECTOR3(1.f, -1.f, 0.f);
	m_vPoint[3] = D3DXVECTOR3(-1.0f, -1.f, 0.0f);

	m_vPoint[4] = D3DXVECTOR3(-1.f, 1.f, 1.f);
	m_vPoint[5] = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_vPoint[6] = D3DXVECTOR3(1.f, -1.f, 1.f);
	m_vPoint[7] = D3DXVECTOR3(-1.0f, -1.f, 1.f);
}

void CFrustum::Transform_ToView(void)
{
	Init_FrustumInProj();

	D3DXMATRIX			matProj;
	matProj = CCamera::GetInstance()->m_matProj;

	D3DXMatrixInverse(&matProj, NULL, &matProj);

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matProj);
	}
}

void CFrustum::Transform_ToWorld(void)
{
	D3DXMATRIX			matView;

	matView = CCamera::GetInstance()->m_matView;

	D3DXMatrixInverse(&matView, NULL, &matView);

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matView);
	}
}

