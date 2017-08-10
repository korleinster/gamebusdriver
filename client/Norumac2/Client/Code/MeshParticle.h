#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CDynamicMesh;
class CMeshParticle :
	public CObj
{
public:
	CMeshParticle();
	virtual ~CMeshParticle();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	D3DXVECTOR3		m_vOriginPos;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(const TCHAR* pMeshKey,D3DXVECTOR3 vPos , PARTICLECOLOR ePColor);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);

public:
	float m_fRotSpeed;
	float m_fMoveSpeed;

public:
	static CMeshParticle* Create(const TCHAR* pMeshKey, D3DXVECTOR3 vPos, PARTICLECOLOR ePColor);

private:
	HRESULT	AddComponent(const TCHAR* pMeshKey, PARTICLECOLOR ePColor);
	void SetCurrling(void);
};