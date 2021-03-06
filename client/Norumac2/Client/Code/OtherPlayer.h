#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTerrainCol;
class CDynamicMesh;
class COtherPlayer :
	public CObj
{
public:
	COtherPlayer();
	virtual ~COtherPlayer();

private:
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CShader*		m_pShadowAniVertexShader;
	CShader*		m_pShadowNonAniVertexShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;
	float			m_fSeverTime;
	PLAYER_STATE	m_ePlayerState;

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	bool			m_bKey; // Ű�� ���ǳ�?
	//bool			m_bPush;
	float			m_fAniTime;
	CVIBuffer*		m_pBuffer;



public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);
	

public:
	static COtherPlayer* Create(void);
	PLAYER_STATE GetAniState(void) { return m_ePlayerState; }
	void SetAniState(PLAYER_STATE eState) { m_ePlayerState = eState; }

private:
	HRESULT	AddComponent(void);
	void ChangeDir(void);
	void SetCurrling(void);
	void SetSeverPosMove(void);
	void AniTimeSet(void);
};

