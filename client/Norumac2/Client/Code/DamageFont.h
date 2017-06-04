#ifndef DmgView_h__
#define DmgView_h__

#include "stdafx.h"
#include "UI.h"
#include "Include.h"


class CResourceMgr;
class CInfo;
class CTexture;
class CVIBuffer;
class CDamageFont : public CUI
{
public:
	CDamageFont(void);
	virtual ~CDamageFont(void);
public:
	virtual HRESULT Initialize(const D3DXVECTOR3* pPos, const float& fDmg);
	virtual int Update(void);
	virtual void Render(void);
public:
	static CDamageFont* Create(const D3DXVECTOR3* pPos, const float& fDmg);
private:
	D3DXVECTOR3				m_vPos;
	int						m_iDmg;
	unsigned int			m_iCnt;
	unsigned int			m_iCnt2;
	float					m_fAddY;
	float					m_fTime;
	float					m_fScaleRatio;
	float					m_fUpSpeed;
	float					m_fRightSpeed;
	float					m_fDrawRatio;
	int						m_iRendNum;

private:
	float					m_fPosRandX;
	float					m_fPosRandY;
	float					m_fAlpha;
	float					m_fMoveSize;

protected:
	CVIBuffer*			m_pBuffer;
	CShader*			m_pShader;
	CTexture*			m_pTexture;

private:
	HRESULT Add_Component(void);
	void Compute_Cnt(void);
};


#endif // DmgView_h__
