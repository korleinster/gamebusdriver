#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTerrainCol;
class CDynamicMesh;
class CBoss :
	public CObj
{
public:
	CBoss();
	virtual ~CBoss();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;
	float			m_fSeverTime;
	BOSS_STATE		m_eBossState;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	bool			m_bKey; // Å°°¡ ´­·Ç³ª?
							//bool			m_bPush;
	list<D3DXVECTOR3> m_SeverPosSaveList;


public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);


public:
	static CBoss* Create(void);
	BOSS_STATE GetAniState(void) { return m_eBossState; }
	void SetAniState(BOSS_STATE eState) { m_eBossState = eState; }

private:
	HRESULT	AddComponent(void);
	void ChangeDir(void);
	void SetCurrling(void);
	void SetSeverPosMove(void);
};

