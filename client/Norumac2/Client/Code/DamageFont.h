#ifndef DmgView_h__
#define DmgView_h__

#include "stdafx.h"
#include "UI.h"
#include "Include.h"


class CFont;
class CDamageFont : public CUI
{
public:
	CDamageFont(void);
	virtual ~CDamageFont(void);
public:
	virtual HRESULT Initialize(const D3DXVECTOR3* pPos, const float& fDmg, const float& fFontSize, const UINT32& nColor);
	virtual int Update(void);
	virtual void Render(void);
public:
	static CDamageFont* Create(const D3DXVECTOR3* pPos, const float& fDmg, const float& fFontSize, const UINT32& nColor);
private:
	D3DXVECTOR3				m_vPoint;
	float					m_fTime;


private:
	float					m_fPosRandX;
	float					m_fPosRandY;
	float					m_fAlpha;
	float					m_fMoveSize;

protected:
	CFont* m_pFont;

private:
	HRESULT Add_Component(void);
};


#endif // DmgView_h__
