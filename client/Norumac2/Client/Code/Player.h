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
	PLAYER_STATE	m_ePlayerState;//애니스테이트
	bool			m_bPush;
	DWORD			m_dwTime;
	bool			m_bMoving;
	bool			m_bPotionCool;//포션 쿨
	float			m_fPotionTime;//포션 쿨
	float			m_fTpTime;//텔레포트 쿨
	bool			m_bTpCool;//텔레포트 쿨
	DWORD			m_dwCellNum;//네비메쉬용
	bool			m_bStart;
	float			m_fComboTime;//콤보 유지시간
	bool			m_bCombo[2];//콤보스택
	bool			m_bMoveSend;// 이동최적화용 임시
	float			m_fSkillMoveTime;//스킬을쓰고 몇초부터 이동을 걸어줄지 하는 시간값
	bool			m_bSkillUsed;//스킬사용여부
	float			m_fChatCool;//채팅전환쿨

	// Scene meshes shader constant buffers
	ID3D11Buffer*		m_pSceneVertexShaderCB;
	ID3D11Buffer*		m_pScenePixelShaderCB;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	ShadowmapRender(void);
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

