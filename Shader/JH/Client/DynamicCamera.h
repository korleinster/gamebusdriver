#pragma once
#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"

class CDynamicCamera : public CCamera
{
public:
	CDynamicCamera();
	virtual ~CDynamicCamera(void);

public:
	HRESULT InitCamera(const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt);

public:
	int Update();

public:
	static CDynamicCamera* Create(const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt);

private:
	void KeyCheck(void);
	void FixMouse(void);
	void MouseMove(void);

private:
	float m_fCamSpeed;
	bool m_bClick;
	bool m_bMouseFix;

};

#endif // DynamicCamera_h__