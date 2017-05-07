#include "stdafx.h"
#include "Line2D.h"
#include "Include.h"

CLine2D::CLine2D(void)
{

}

CLine2D::~CLine2D(void)
{

}

HRESULT CLine2D::Init_Line(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint)
{
	m_vStartPoint = D3DXVECTOR2(pStartPoint->x, pStartPoint->z);
	m_vEndPoint = D3DXVECTOR2(pEndPoint->x, pEndPoint->z);

	D3DXVECTOR2		vDir = m_vEndPoint - m_vStartPoint;
	m_vNormal = D3DXVECTOR2(vDir.y * -1, vDir.x);
	D3DXVec2Normalize(&m_vNormal, &m_vNormal);
	return S_OK;
}

CLine2D::DIR CLine2D::Check_Dir(const D3DXVECTOR2* pMoveEndPoint)
{
	D3DXVECTOR2		vDir = *pMoveEndPoint - m_vStartPoint;

	float		fDotResult = D3DXVec2Dot(&vDir, &m_vNormal);

	if (fDotResult > 0)
		return DIR_LEFT;
	else
		return DIR_RIGHT;
}

CLine2D* CLine2D::Create(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint)
{
	CLine2D*	pLine = new CLine2D;
	if (FAILED(pLine->Init_Line(pStartPoint, pEndPoint)))
		Safe_Release(pLine);

	return pLine;
}

void CLine2D::Release(void)
{

}

