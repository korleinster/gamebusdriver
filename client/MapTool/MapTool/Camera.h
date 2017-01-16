#pragma once
#include "Include.h"


class CInfo;
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


private:
	CInfo* m_pInfo;

private:
	float m_fFovY;
	float m_fAspect;
	float m_fNear;
	float m_fFar;
	float m_fCameraDistance;
	float m_fCameraSpeed;
	bool  m_bMouseFix;
	DWORD m_dwTime;
	DWORD m_dwKey;
	D3DXVECTOR3 m_vDirZ;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Release(void);

public:
	void MakeView(void);
	void MakeProjection(void);

private:
	void KeyState(void);
	void FixMouse(void);
	void MouseMove(void);


};

