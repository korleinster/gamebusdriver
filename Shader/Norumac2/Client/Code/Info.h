#pragma once
#include "Component.h"
class CInfo :
	public CComponent
{
private:
	explicit CInfo(const D3DXVECTOR3& vLook);

public:
	virtual ~CInfo(void);

public:
	virtual int Update(void);
	virtual DWORD Release(void);

public:
	static CInfo* Create(const D3DXVECTOR3& vLook);

public:
	float				m_fAngle[ANGLE_END];
	D3DXVECTOR3			m_vScale;
	D3DXVECTOR3			m_vPos;
	D3DXVECTOR3			m_vDir;
	D3DXMATRIX			m_matWorld;


};

