#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CQuestMark :
	public CObj
{
public:
	CQuestMark();
	virtual ~CQuestMark();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;

public:
	virtual HRESULT Initialize(D3DXVECTOR3 vPos);
	virtual int		Update(void);
	virtual void	Render(bool bQuestAccept);

public:
	static CQuestMark* Create(D3DXVECTOR3 vPos);

private:
	HRESULT	AddComponent(void);



};

