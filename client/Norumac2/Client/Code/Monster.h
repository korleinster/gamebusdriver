#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTerrainCol;
class CDynamicMesh;
class CMonster :
	public CObj
{
public:
	CMonster();
	virtual ~CMonster();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;
	float			m_fSeverTime;
	PLAYER_STATE	m_ePlayerState;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	bool			m_bKey; // Å°°¡ ´­·Ç³ª?
							//bool			m_bPush;


public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);


public:
	static CMonster* Create(void);
	PLAYER_STATE GetAniState(void) { return m_ePlayerState; }
	void SetAniState(PLAYER_STATE eState) { m_ePlayerState = eState; }

private:
	HRESULT	AddComponent(void);
};

