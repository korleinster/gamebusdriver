#include "stdafx.h"
#include "Include.h"
#include "NaviCell.h"
#include "LineCol.h"
#include "Line2D.h"
#include "Camera.h"
#include "Device.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "Info.h"

CNaviCell::CNaviCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC)
	: m_dwIndex(0)
	, m_dwType(0)
{
	ZeroMemory(m_pNeighbor, sizeof(CNaviCell*) * NEIGHBOR_END);
	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;
	m_pBuffer = NULL;
	m_pPixelShader = NULL;
	m_pVertexShader = NULL;

}

CNaviCell::~CNaviCell(void)
{

}

D3DXVECTOR3* CNaviCell::GetPoint(POINT ePointID)
{
	return &m_vPoint[ePointID];
}

DWORD CNaviCell::GetType(void)
{
	return m_dwType;
}

CNaviCell* CNaviCell::GetNeighbor(NEIGHBOR eNeighbor)
{
	return m_pNeighbor[eNeighbor];
}

DWORD CNaviCell::GetIndex(void)
{
	return m_dwIndex;
}

CLine2D* CNaviCell::GetLine(LINE eLineID)
{
	return m_pLine2D[eLineID];
}

void CNaviCell::SetPoint(D3DXVECTOR3 vDest, POINT ePointID)
{
	m_vPoint[ePointID] = vDest;
}

DWORD CNaviCell::GetCellType(void)
{
	return m_dwType;
}

void CNaviCell::SetNeighbor(NEIGHBOR eNeighborID, CNaviCell* pNeighbor)
{
	m_pNeighbor[eNeighborID] = pNeighbor;
}

void CNaviCell::SetIndex(DWORD dwIndex)
{
	m_dwIndex = dwIndex;
}

HRESULT CNaviCell::Init_Cell(const DWORD& dwIdx, const DWORD& dwType)
{
	m_dwIndex = dwIdx;
	m_dwType = dwType;

	m_pLine2D[LINE_AB] = CLine2D::Create(&m_vPoint[POINT_A], &m_vPoint[POINT_B]);
	m_pLine2D[LINE_BC] = CLine2D::Create(&m_vPoint[POINT_B], &m_vPoint[POINT_C]);
	m_pLine2D[LINE_CA] = CLine2D::Create(&m_vPoint[POINT_C], &m_vPoint[POINT_A]);


	D3DXMatrixIdentity(&m_CellMat);
	D3DXMATRIX	matScale, matTrans;

	//스자이공
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.f);

	m_CellMat = matScale * matTrans;


	return S_OK;
}

void CNaviCell::Render(void)
{
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_CellMat);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	CDevice::GetInstance()->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	CDevice::GetInstance()->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	//////////////////
	CDevice::GetInstance()->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);


	m_pBuffer->Render();
}

CNaviCell* CNaviCell::Create(const D3DXVECTOR3* pPointA
	, const D3DXVECTOR3* pPointB
	, const D3DXVECTOR3* pPointC
	, const DWORD& dwIdx
	, const DWORD& dwType)
{
	CNaviCell*	pCell = new CNaviCell(pPointA, pPointB, pPointC);
	if (FAILED(pCell->Init_Cell(dwIdx, dwType)))
		Safe_Release(pCell);

	return pCell;
}

void CNaviCell::Release(void)
{
	for (int i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine2D[i]);

	Safe_Release(m_pPixelShader);
	Safe_Release(m_pVertexShader);

}

bool CNaviCell::ComparePoint(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, CNaviCell* pNeighbor)
{
	if (*pPointA == m_vPoint[POINT_A])
	{
		if (*pPointB == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (*pPointB == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
	}

	if (*pPointA == m_vPoint[POINT_B])
	{
		if (*pPointB == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (*pPointB == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	if (*pPointA == m_vPoint[POINT_C])
	{
		if (*pPointB == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
		else if (*pPointB == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}
	return false;
}

bool CNaviCell::CheckPass(const D3DXVECTOR3* pPos
	, const D3DXVECTOR3* pDir
	, NEIGHBOR* peNeighborID)
{
	for (int i = 0; i < 3; ++i)
	{
		D3DXVECTOR2		vEndPoint = D3DXVECTOR2(pPos->x + pDir->x, pPos->z + pDir->z);
		if (CLine2D::DIR_LEFT == m_pLine2D[i]->Check_Dir(&vEndPoint))
		{
			*peNeighborID = NEIGHBOR(i);
			return true;
		}
	}

	return false;
}
bool CNaviCell::CheckPass(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, DWORD dwNeighborID)
{
	D3DXVECTOR2		vEndPoint = D3DXVECTOR2(pPos->x + pDir->x, pPos->z + pDir->z);
	if (CLine2D::DIR_LEFT == m_pLine2D[dwNeighborID]->Check_Dir(&vEndPoint))
	{
		return true;
	}

	return false;
}

D3DXVECTOR3 CNaviCell::SlidingVector(D3DXVECTOR3& vDir, DWORD dwNeighborID)
{
	D3DXVECTOR2 vTemp = D3DXVECTOR2(vDir.x, vDir.z);
	D3DXVECTOR2 vOut;
	vOut = vTemp - m_pLine2D[(LINE)dwNeighborID]->m_vNormal * D3DXVec2Dot(&m_pLine2D[(LINE)dwNeighborID]->m_vNormal, &vTemp);
	D3DXVECTOR3 vOut3 = D3DXVECTOR3(vOut.x, 0.f, vOut.y);
	return vOut3;
}

HRESULT CNaviCell::AddComponent(CLineCol* pLineCol)
{
	m_pBuffer = pLineCol;
	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_LINE");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS_LINE");

	return S_OK;
}
