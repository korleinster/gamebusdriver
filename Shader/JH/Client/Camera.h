#pragma once
#include "Include.h"

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

public:
	DECLARE_SINGLETON(CCamera)

public:
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProj;
	D3DXVECTOR3 m_vEye;
	D3DXVECTOR3 m_vAt;
	D3DXVECTOR3 m_vUp;

protected:
	float m_fFovY;
	float m_fAspect; 
	float m_fNear;
	float m_fFar;

public:
	HRESULT Initialize(void);
	int		Update(void);

public:
	void MakeView(void);
	void MakeProjection(void);

private:
	void Release(void);



};

