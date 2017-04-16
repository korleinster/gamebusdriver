#pragma  once

#include "Include.h"

class CFrustum
{
	DECLARE_SINGLETON(CFrustum)
public:
	CFrustum();
	~CFrustum();
public:
	bool Decision_In(const D3DXVECTOR3* pPosition, const float& fRadius);
	bool Decision_In(const D3DXVECTOR3* pPosition);
	bool Decision_In_Object(const D3DXVECTOR3* pPosition, const float& fRadius);

private:
	D3DXVECTOR3				m_vPoint[8];
	D3DXPLANE			m_Plane[6];
private:
	void Init_FrustumInProj(void);
	void Transform_ToView(void);
	void Transform_ToWorld(void);

};