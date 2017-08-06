#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CDynamicMesh;
class CTerrainCol;
class CNpc :
	public CObj
{
public:
	CNpc();
	virtual ~CNpc();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CShader*		m_pShadowAniVertexShader;
	CShader*		m_pShadowNonAniVertexShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);



public:
	static CNpc* Create(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos);

private:
	HRESULT	AddComponent(wstring wstMeshKey, wstring wstrTextureKey);
	void SetCurrling(void);
};

