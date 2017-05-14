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
	D3DXVECTOR3 m_vTarget;


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
	bool  m_bDebugCam;
	DWORD m_dwTime;
	DWORD m_dwKey;
	D3DXVECTOR3 m_vDirZ;
	CInfo* m_pTargetInfo;
	float m_fAngle;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Release(void);

public:
	const D3DXMATRIX* GetViewMatrix() { return &m_matView; }
	const D3DXMATRIX* GetProjMatrix() { return &m_matProj; }

	void MakeView(void);
	void MakeProjection(void);
	void SetCameraTarget(CInfo* pInfo);

private:
	void KeyState(void);
	void FixMouse(void);
	void MouseMove(void);
	void TargetRenewal(void);


};

