#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CDynamicMesh;
class CEffect :
	public CObj
{
public:
	CEffect();
	virtual ~CEffect();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);

public:
	float m_fRenderTime;


public:
	static CEffect* Create(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos);

private:
	HRESULT	AddComponent(wstring wstMeshKey, wstring wstrTextureKey);
	void ChangeDir(void);
	void SetCurrling(void);
	void SetSeverPosMove(void);
};

