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
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	CTerrainCol*	m_pTerrainCol;
	player_data		m_ServerInfo;


public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);
	player_data*	GetPacketData(void);

public:
	static COtherPlayer* Create(void);

private:
	void	Release(void);
	HRESULT	AddComponent(void);
};
