#pragma once
#include "Include.h"


class CLine2D
{
public:
	enum DIR { DIR_LEFT, DIR_RIGHT };

private:
	CLine2D(void);
	~CLine2D(void);

public:
	HRESULT Init_Line(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint);
	DIR Check_Dir(const D3DXVECTOR2* pMoveEndPoint);

public:
	D3DXVECTOR2		m_vStartPoint;
	D3DXVECTOR2		m_vEndPoint;
	D3DXVECTOR2		m_vNormal;

public:
	static CLine2D* Create(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint);
	void Release(void);
};
