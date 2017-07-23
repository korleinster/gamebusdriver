#pragma once
#include "Obj.h"

class CVIBuffer;
class CComponent;
class CShader;
class CTexture;
class CTerrain :
	public CObj
{
private:
	CTerrain();
public:
	virtual ~CTerrain();

private:
	CVIBuffer*	m_pTerrainBuffer;
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;
	CTexture*	m_pTexture;
	VTXTEX*		m_pVerTex;
	VTXTEX*		m_pConvertVerTex;
	DWORD		m_dwTriCnt;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	static CTerrain* Create(void);

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);

public:
	VTXTEX**	GetVertex(void) { return &m_pVerTex; }

private:
	HRESULT	AddComponent(void);

};

