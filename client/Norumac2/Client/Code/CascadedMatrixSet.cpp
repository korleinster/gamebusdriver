//#include "DXUT.h"
//#include "DXUTCamera.h"
#include "stdafx.h"
#include "CascadedMatrixSet.h"
#include "Camera.h"

//extern float g_fCameraFOV;
//extern float g_fAspectRatio;
//extern CFirstPersonCamera g_Camera;

CCascadedMatrixSet::CCascadedMatrixSet() : m_bAntiFlickerOn(true), m_fCascadeTotalRange(40.0f), m_pCamera(CCamera::GetInstance())
{

}

CCascadedMatrixSet::~CCascadedMatrixSet()
{

}

void CCascadedMatrixSet::Init(int iShadowMapSize)
{
	m_iShadowMapSize = iShadowMapSize;

	// Set the range values
	m_arrCascadeRanges[0] = m_pCamera->m_fNear;
	m_arrCascadeRanges[1] = 1.5f;
	m_arrCascadeRanges[2] = 5.0f;
	m_arrCascadeRanges[3] = m_fCascadeTotalRange;

	for(int i = 0; i < m_iTotalCascades; i++)
	{
		m_arrCascadeBoundCenter[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_arrCascadeBoundRadius[i] = 0.0f;
	}
}

void CCascadedMatrixSet::Update(const D3DXVECTOR3& vDirectionalDir)
{
	// Find the view matrix
	D3DXVECTOR3 vWorldCenter = m_pCamera->m_vAt;//m_pCamera->m_vEye + m_pCamera->GetWorldAhead() * m_fCascadeTotalRange * 0.5f;
	D3DXVECTOR3 vPos = vWorldCenter;
	D3DXVECTOR3 vLookAt = vWorldCenter - (vDirectionalDir * m_pCamera->m_fFar * 0.1f);
	D3DXVECTOR3 vUp;
	D3DXVECTOR3 vRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXVec3Cross(&vUp, &vDirectionalDir, &vRight);
	D3DXVec3Normalize( &vUp, &vUp );
	D3DXMATRIX mShadowView;
	D3DXMatrixLookAtLH( &mShadowView, &vLookAt, &vPos, &vUp);

	// Find the projection matrix
	D3DXMatrixOrthoLH(&m_ShadowSpaceProj, m_fCascadeTotalRange, m_fCascadeTotalRange, m_pCamera->m_fNear, m_pCamera->m_fFar);

	// The combined transformation from world to shadow space
	m_WorldToShadowSpace = mShadowView * m_ShadowSpaceProj;
}
