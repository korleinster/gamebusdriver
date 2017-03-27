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
	VTXTEX*			m_pVerTex;


public:
	virtual HRESULT Initialize(const TCHAR* pMeshKey);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);
	Packet*			GetPacket(void);

public:
	static CStaticObject* Create(const TCHAR* pMeshKey);
	void	Release(void);

private:
	HRESULT	AddComponent(const TCHAR* pMeshKey, const TCHAR* pTextureKey);
};

