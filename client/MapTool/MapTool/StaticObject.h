#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CDynamicMesh;
class CStaticObject :
	public CObj
{
public:
	CStaticObject();
	virtual ~CStaticObject();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	//VTXTEX*			m_pVerTex;

public:
	HRESULT Initialize(const TCHAR* pMeshKey, D3DXVECTOR3 vPos);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);

public:
	static CStaticObject* Create(const TCHAR* pMeshKey, D3DXVECTOR3 vPos);
	void	Release(void);

private:
	HRESULT	AddComponent(const TCHAR* pMeshKey, const TCHAR* pTextureKey);
};

