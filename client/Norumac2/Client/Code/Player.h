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


public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);
	Packet*			GetPacket(void);
	void			AniMove(void);

public:
	static CPlayer* Create(void);
	void	Release(void);

private:
	HRESULT	AddComponent(void);
};

