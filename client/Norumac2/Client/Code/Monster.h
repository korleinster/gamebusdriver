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
	MONSTER_STATE	m_eMonsterState;

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
	MONSTER_STATE GetAniState(void) { return m_eMonsterState; }
	void SetAniState(MONSTER_STATE eState) { m_eMonsterState = eState; }

private:
	HRESULT	AddComponent(void);
	void ChangeDir(void);
	void SetCurrling(void);
};

