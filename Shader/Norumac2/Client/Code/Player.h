#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTerrainCol;
class CPlayer :
	public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

private:
	CVIBuffer*			m_pBuffer;
	CShader*			m_pVertexShader;
	CShader*			m_pPixelShader;
	CTexture*			m_pTexture;
	VTXTEX*				m_pVerTex;
	CTerrainCol*		m_pTerrainCol;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);

public:
	static CPlayer* Create(void);

private:
	void	Release(void);
	HRESULT	AddComponent(void);
};

