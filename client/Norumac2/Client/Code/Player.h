#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTerrainCol;
class CDynamicMesh;
class CPlayer :
	public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;
	Packet*			m_Packet;
	float			m_fSeverTime;
	bool			m_bSendServer;
	PLAYER_STATE	m_ePlayerState;
	bool			m_bPush;
	DWORD			m_dwTime;
	bool			m_bMoving;
	bool			m_bPotionCool;
	float			m_fPotionTime;
	float			m_fTpTime;
	bool			m_bTpCool;
	DWORD			m_dwCellNum;
	bool			m_bStart;
	float			m_fComboTime;
	bool			m_bCombo[2];

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);
	Packet*			GetPacket(void);
	void			AniMove(void);
	void			SetNaviIndex(DWORD dwIndex) { m_dwCellNum = dwIndex; }
	void			TimeSetter(void);

public:
	static CPlayer* Create(void);

private:
	HRESULT	AddComponent(void);
};

